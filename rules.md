# Coding Rules & Guardrails — EdgeX-GUI

## Architecture Rules

### One Process Per Profile — Non-Negotiable
The Axelera SDK's `create_inference_stream()` and its underlying GStreamer/AIPU native code
is **not re-entrant**. Calling it from two threads in the same process causes a **segfault**
in the SDK's native libraries. Never try to run two profiles in a single server process.

```
✅ CORRECT: axelera_server.py (process 1, port 5567) handles YOLO
            axelera_server.py (process 2, port 5570) handles Face
❌ WRONG:   one axelera_server.py handles both YOLO + Face via two stream objects
```

### Stream Creation Must Be Serialised
Even within one process, the `_stream_create_lock` must wrap `create_inference_stream()`.
This is a defensive guard; do not remove it.

### SDK Monkey-Patches Must Stay
Two patches are applied at module import time in `axelera_server.py`:
1. `_patched_configure_boards` — core offset partitioning + configure_device timeout
2. `_patched_build_gst_usb` — forces `io-mode=2` (mmap) on all v4l2src elements

Do not remove either patch. Do not move them into functions (must run at import time).

## Python Rules

### SDK Path Must Be Stripped
```python
# Always .strip() the env var — CRLF from Windows-edited shell scripts corrupts the path
_sdk_root = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk").strip()
sys.path.insert(0, _sdk_root)   # insert at front, not append
```

### Per-Profile State Dict
Profile state lives in `_profiles: dict[str, dict]`. Key = profile name string.
Value = `{"stream": obj|None, "stop_event": Event, "done_event": Event}`.
Always acquire `_profiles_lock` before reading or writing this dict.

### Thread-Local Core Assignment
Each inference thread stores its AIPU core assignment in thread-local storage (`_tls`):
```python
_tls.core_offset = int(core_list[0])   # first core number e.g. 0 or 2
_tls.core_limit  = len(core_list)      # number of cores e.g. 2
```
The monkey-patched `_configure_boards` reads these. Set them before calling `create_inference_stream`.

### Command Parsing
The GUI sends `start:{json_no_profile_name}` (no profile name). The server detects this:
```python
body = after_cmd.strip()
if body.startswith("{"):
    profile_name = "default"
    cfg_str = body
else:
    brace = body.find("{")
    profile_name = body[:brace].rstrip(":").strip()
    cfg_str = body[brace:]
```
Do not use `raw.split(":", 2)` — it splits JSON at its first internal colon.

## Shell Script Rules

### Always Use Unix Line Endings
Shell scripts edited on Windows MUST be converted before deployment:
```bash
dos2unix /data/edgex-gui/setup_axelera_env.sh /data/edgex-gui/start_backend.sh
```
CRLF in bash `export` statements embeds `\r` in env var values, which silently corrupts
Python paths (`/data/voyager-sdk\r` is not a valid path).

### Do Not Use `set -e` in Test Scripts
`pkill` returns exit code 1 when no processes match. With `set -e`, the script exits silently.
Use `pkill ... || true` instead.

### Background Processes in SSH
`nohup cmd &` in an SSH session dies when the session closes unless you also `disown`.
Preferred pattern for remote background execution:
```bash
cmd > logfile 2>&1 &
disown
```

## GStreamer Rules

### Camera Device Nodes
USB cameras expose multiple V4L2 nodes. Only even-numbered nodes capture video:
- `/dev/video20` → Brio #1 video capture ✅
- `/dev/video21` → Brio #1 metadata only ❌ (no frames, causes `Failed to set state`)
- `/dev/video22` → Brio #2 video capture ✅

### USB Bandwidth
Two Brio 100 cameras on the same USB controller cannot both run at full resolution (1080p MJPEG).
If both must run simultaneously, configure at least one to 640×480:
```python
# In _patched_build_gst_usb, force lower caps when needed:
dimensions = f'width={src.width},height={src.height}' if src.width and src.height else 'width=640,height=480'
framerate  = f'framerate={src.fps}/1' if src.fps else 'framerate=30/1'
```

### io-mode Must Be mmap for Multi-Camera
The Axelera downstream buffer pool uses DMA-BUF. When two USB cameras run simultaneously,
the DMA-BUF allocation pool is exhausted on the second camera. `io-mode=2` (mmap) forces
each v4l2src to use kernel mmap'd buffers (regular RAM), avoiding this contention.

## AIPU Rules

### AXELERA_CONFIGURE_BOARD=0 Is Required for Process 2+
If a second server process calls `configure_device()` while another inference process is running,
the driver mutex blocks indefinitely. The firmware boot flag mechanism prevents this:
- First process: runs configure_device (possibly in a 12s-timeout thread), then creates `/tmp/.axelera_board_fw_loaded`
- Every later process: `main()` sees the flag and sets `AXELERA_CONFIGURE_BOARD=0` before any SDK imports

### Do Not Flash Firmware Twice
`device_firmware=1` in the configure_device call re-flashes the entire chip. This blocks all
inference for several seconds. The boot flag prevents duplicate flashing within a session.

## Testing Rules

### Always Test Multi-Process, Never Single-Process for Multi-Camera
Two-camera validation must use two separate server processes (one per profile), not two
start commands to the same server.

### Use the Correct Start Command Format
```bash
# Correct — server understands this:
printf 'start:{"modelPath":"yolov8n-coco-onnx","cameraSource":"usb:20","aipuCores":"0,1"}\n' | nc -q2 127.0.0.1 5567

# Wrong — missing "start:" prefix:
echo '{"modelPath":"yolov8n-coco-onnx",...}' | nc 127.0.0.1 5567
```

### Wait Long Enough for SDK Init
The AIPU board initialisation on the first process takes ~8 seconds. Face recognition model
loading takes ~4 seconds. Add sufficient sleep between "start the server" and "send start command":
- First process boot: `sleep 8` minimum
- After `start` command (waiting for inference to begin): `sleep 12-15` minimum
