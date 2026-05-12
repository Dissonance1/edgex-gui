# EdgeX-GUI — Multi-Camera AI Inference Dashboard

A Qt C++ GUI for running real-time multi-camera AI inference on the **Axelera Metis AIPU** (Aetina AN110, aarch64). The GUI manages one `axelera_server.py` process per camera profile; each process drives a GStreamer-based inference pipeline on its own AIPU core partition and streams video frames and metadata back to the GUI over TCP.

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│  Host (dev machine / WSL)                                                       │
│                                                                                 │
│   Qt GUI  (edgex-qt-ui — C++)                                                   │
│   ┌─────────────────────────────────────────────────────────────────────────┐   │
│   │  MainWindow                                                             │   │
│   │  ├── AIRuntimeView          ← profile management UI                    │   │
│   │  ├── LiveMonitoringWidget   ← multi-camera video grid                  │   │
│   │  ├── VideoFrameWidget       ← per-camera JPEG renderer                 │   │
│   │  ├── DashboardView          ← EdgeX service status                     │   │
│   │  ├── DevicesView            ← EdgeX device registry                    │   │
│   │  ├── EventsReadingsView     ← EdgeX events / readings                  │   │
│   │  ├── NotificationView       ← EdgeX notifications                      │   │
│   │  └── RulesEngineView        ← EdgeX rules engine                       │   │
│   │                                                                         │   │
│   │  AIInferenceWorker (per profile)                                        │   │
│   │  ├── spawns axelera_server.py via QProcess                             │   │
│   │  ├── CMD  socket → port 5567/5570/5573  (send commands)                │   │
│   │  ├── VIDEO socket ← port 5568/5571/5574  (receive JPEG stream)         │   │
│   │  └── META  socket ← port 5566/5569/5572  (receive JSON metadata)       │   │
│   └─────────────────────────────────────────────────────────────────────────┘   │
│                          │ QProcess + TCP                                        │
└──────────────────────────┼──────────────────────────────────────────────────────┘
                           │ (on Aetina AN110 aarch64)
┌──────────────────────────▼──────────────────────────────────────────────────────┐
│  axelera_server.py  (one process per camera profile)                            │
│                                                                                 │
│  ┌──────────────┐  ┌───────────────────┐  ┌───────────────────────────────┐   │
│  │  CMD server  │  │   VIDEO server    │  │      META server              │   │
│  │  TCP :5567   │  │   TCP :5568       │  │      TCP :5566                │   │
│  │              │  │                   │  │                               │   │
│  │  start:{...} │  │  JPEG frames →    │  │  JSON detections →            │   │
│  │  stop        │  │  GUI clients      │  │  GUI clients                  │   │
│  │  status      │  └───────────────────┘  └───────────────────────────────┘   │
│  │  shutdown    │                                                               │
│  └──────┬───────┘                                                               │
│         │                                                                       │
│  ┌──────▼────────────────────────────────────────────────────────────────────┐ │
│  │  GStreamer Pipeline                                                        │ │
│  │  camera source → decode → axinplace (AIPU inference) → JPEG encode        │ │
│  │                              │                                             │ │
│  │  AXELERA_CORE_OFFSET=N  ─────┘  (core partitioning via SDK monkey-patch)  │ │
│  └──────────────────────────────────────────────────────────────────────────┘ │
│                                                                                 │
│  Axelera Metis AIPU  (4 cores total)                                            │
│  ┌───────┐  ┌───────┐  ┌───────┐  ┌───────┐                                   │
│  │ Core0 │  │ Core1 │  │ Core2 │  │ Core3 │                                   │
│  │ Cam 1 │  │ Cam 1 │  │ Cam 2 │  │ Cam 2 │  ← partitioned per profile       │
│  └───────┘  └───────┘  └───────┘  └───────┘                                   │
└─────────────────────────────────────────────────────────────────────────────────┘
                           │
                           │ HTTP POST (optional)
                           ▼
              EdgeX Foundry Core Data API
              (inference results as EdgeX events)
```

### Multi-Process Core Partitioning

The Axelera SDK's `DeviceManager` hard-codes `core_index = 0` per process. `axelera_server.py` monkey-patches it to read `AXELERA_CORE_OFFSET` from the environment, which `AIInferenceWorker` sets from the profile's `aipuCores` field. Board firmware is initialised exactly once (12-second timeout) by the first process; subsequent processes set `AXELERA_CONFIGURE_BOARD=0` to skip it.

### Port Assignments

| Profile | CMD  | VIDEO | META |
|---------|------|-------|------|
| 1       | 5567 | 5568  | 5566 |
| 2       | 5570 | 5571  | 5569 |
| 3       | 5573 | 5574  | 5572 |

---

## Prerequisites

### Build machine

| Dependency | Version |
|-----------|---------|
| Qt        | 5.15+ or 6.x (Widgets, Network, Multimedia, MultimediaWidgets) |
| CMake     | 3.16+ |
| C++ compiler | C++20 (GCC 11+, Clang 13+) |
| GStreamer | 1.0 (gstreamer-app, gstreamer-video) |
| OpenCV    | 4.x |
| Axelera SDK | axruntime, axstreamer (Voyager SDK) |

### Target device (Aetina AN110 aarch64)

- Axelera Metis AIPU driver loaded
- Voyager SDK at `/data/voyager-sdk/`
- Python venv at `/data/voyager-sdk/venv/bin/python3`

---

## Build

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
# Binary: build/bin/edgex-qt-ui
```

For aarch64 cross-compilation the CMakeLists sets RPATH to `/data/voyager-sdk/operators/lib` and `/opt/axelera/runtime-1.5.2-1/lib` automatically.

To produce a `.deb` package:

```bash
cd build && cpack
```

---

## Deploy to Aetina AN110

```bash
# From the repo root on the dev machine:
bash sync_to_aetina.sh

# Or sync only the server (faster iteration):
wsl -d Ubuntu bash -c "rsync -avz --exclude '__pycache__' -e 'ssh -o StrictHostKeyChecking=no' \
    /home/rog_strix/src/edgex-gui/axelera_server.py aetina@10.12.83.250:/data/edgex-gui/"
```

> **Line-ending warning:** `setup_axelera_env.sh` and `start_backend.sh` must have Unix line endings.
> After syncing from Windows run: `dos2unix /data/edgex-gui/setup_axelera_env.sh /data/edgex-gui/start_backend.sh`

---

## Run

### On the Aetina device

```bash
cd /data/edgex-gui
source ./setup_axelera_env.sh
/data/voyager-sdk/venv/bin/python3 axelera_server.py \
    --cmd-port 5567 --video-port 5568 --meta-port 5566 \
    --log-file /tmp/srv.log
```

### From the GUI

Launch `edgex-qt-ui`. The **AI Runtime** panel lets you add camera profiles (model, source, cores), start/stop them, and view live video + detection metadata.

### Running on WSL (dev machine)

Ensure an X server (GWSL or VcXsrv) is running, then:

```bash
export DISPLAY=:0
./build/bin/edgex-qt-ui
```

---

## Server Command Protocol

Commands are newline-terminated strings sent to the CMD port (default 5567):

```
start:{json}\n                 # start single profile (name = "default")
start:ProfileName:{json}\n     # start named profile
stop\n                         # stop all profiles
stop:ProfileName\n             # stop one profile
toggle_video on|off\n          # enable / disable JPEG encoding
status\n                       # returns running profile names as JSON
shutdown\n                     # graceful exit
```

Example `start` payload:

```json
{
  "modelPath": "yolov8n-coco-onnx",
  "cameraSource": "usb:0",
  "aipuCores": "0,1",
  "confidenceThreshold": 0.5
}
```

---

## Camera & Model Configuration

`config_multi_camera.json` defines multiple streams:

```json
{
  "streams": [
    {
      "id": 1,
      "cameraSource": "usb:0",
      "modelPath": "yolov8n-coco-onnx",
      "confidenceThreshold": 0.5,
      "aipuCores": "0,1",
      "edgeX": { "deviceName": "Gate-01", "profileName": "AI-Entry" }
    },
    {
      "id": 2,
      "cameraSource": "usb:1",
      "modelPath": "yolov8n-coco-onnx",
      "confidenceThreshold": 0.6,
      "aipuCores": "2,3"
    }
  ]
}
```

`cameraSource` accepts `usb:<index>`, `rtsp://<url>`, or a file path.

---

## Testing

```bash
# Two-process, two-camera integration test (on device):
bash /data/edgex-gui/test_multiprocess.sh

# Send a start command manually:
printf 'start:{"modelPath":"yolov8n-coco-onnx","cameraSource":"usb:0","aipuCores":"0,1","confidenceThreshold":0.5}\n' \
    | nc -q2 127.0.0.1 5567

# Check status:
echo 'status' | nc -q1 127.0.0.1 5567

# View logs:
cat /data/edgex-gui/logs/profile_default_*.log
```

---

## Common Operations

```bash
# Kill all running server processes on device:
wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 'pkill -f axelera_server || true'"

# Clear AIPU boot flag (force re-initialisation on next start):
wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 'rm -f /tmp/.axelera_board_fw_loaded'"

# View recent log files on device:
wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 'ls -lt /data/edgex-gui/logs/ | head -10'"
```

---

## Key Source Files

| File | Purpose |
|------|---------|
| `axelera_server.py` | Python inference server — CMD/VIDEO/META TCP servers, GStreamer pipeline, AIPU core partitioning |
| `src/AIInferenceWorker.cpp` | Qt worker that spawns and manages server processes, relays video & metadata |
| `src/AIRuntimeView.cpp` | UI panel for adding/starting/stopping camera profiles |
| `src/LiveMonitoringWidget.cpp` | Multi-camera live video grid |
| `src/VideoFrameWidget.cpp` | Per-camera JPEG frame renderer |
| `src/DataIngestionServer.cpp` | TCP server for inbound EdgeX data |
| `setup_axelera_env.sh` | Sets `PYTHONPATH`, `LD_LIBRARY_PATH`, `GST_PLUGIN_PATH` |
| `start_backend.sh` | Sources env then execs the server |
| `sync_to_aetina.sh` | Rsync key files to device |
| `config_multi_camera.json` | Multi-stream config template |

---

## EdgeX Foundry Integration (optional)

When an `edgeX` block is present in a stream config, inference results are POST-ed to EdgeX Core Data as events. Use `edgex_proxy.py` (port 4000 → localhost:59880) when the EdgeX stack is running locally for development.

---

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Server crashes on second `start` | SDK stream creation is not re-entrant; the lock in `_stream_create_lock` serialises it — check logs for deadlock |
| No video frames in GUI | Check `toggle_video` state; confirm VIDEO port matches profile assignment |
| `configure_device` hangs | Another process owns core 0; set `AXELERA_CONFIGURE_BOARD=0` or clear `/tmp/.axelera_board_fw_loaded` |
| CRLF errors in shell scripts | Run `dos2unix setup_axelera_env.sh start_backend.sh` on the device |
| Display errors (WSL build) | Set `DISPLAY=$(grep nameserver /etc/resolv.conf \| awk '{print $2}'):0` |
