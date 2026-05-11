# Reusable Prompts — EdgeX-GUI

Paste these directly into Claude Code when you need common tasks done.

---

## Deploy and test a code change

```
I've edited axelera_server.py locally. Please:
1. Rsync only axelera_server.py to aetina@10.12.83.250:/data/edgex-gui/
2. Run the multi-process two-camera test on the device (/tmp/test_multiprocess.sh)
3. Show me the YOLO server log and FACE server log
4. Tell me if both streams came online or what error appeared
```

---

## Debug a new inference failure

```
The <PROFILE_NAME> profile is failing. The log is at /data/edgex-gui/logs/<filename>.
Please read the log and tell me:
- At what stage it fails (AIPU init / GStreamer pipeline / camera open / inference loop)
- What the exact error message is
- Whether it's a first-run issue (configure_device) or a recurring crash
- What fix to try based on context.md and rules.md
```

---

## Add a new camera profile

```
I want to add a new profile called "<NAME>" that runs:
  Model: <model_path>
  Camera: usb:<N>
  AIPU cores: <X,Y>
  Confidence: <0.X>

Using the existing profile structure in axelera_server.py and config_multi_camera.json,
add this profile. Make sure:
- The camera device node is a capture node (even-numbered /dev/videoN)
- The AIPU cores don't overlap with other running profiles
- Port assignments follow the table in CLAUDE.md
```

---

## Investigate a USB camera issue

```
Camera usb:<N> is failing with a GStreamer error. Please:
1. SSH to the device and run: v4l2-ctl --list-devices and v4l2-ctl -d /dev/video<N> --list-formats-ext
2. Check dmesg for USB errors: ssh aetina 'dmesg | tail -50'
3. Check if the device node is a capture node (not metadata)
4. Tell me what formats and resolutions the camera supports
5. Suggest whether to use usb:<N> or usb:<N+2> based on the results
```

---

## Fix CRLF issues after Windows sync

```
After syncing from Windows, shell scripts on the device may have CRLF line endings
which corrupt PYTHONPATH. Please:
1. SSH to aetina and run: file /data/edgex-gui/setup_axelera_env.sh /data/edgex-gui/start_backend.sh
2. If either shows "CRLF line terminators", run dos2unix on both files
3. Verify they now show "ASCII text executable" without CRLF
```

---

## Check AIPU core partitioning is working

```
I want to verify that the AIPU core monkey-patch is assigning cores correctly.
Please start two servers (YOLO on port 5567 with AXELERA_CORE_OFFSET=0, 
Face on port 5570 with AXELERA_CORE_OFFSET=2), send start commands, then 
check the logs for these lines:
- YOLO: "Hardware Offset: 0" and "Allocated Core Range: [0, 1]"  
- Face: "Hardware Offset: 2" and "Hardware Offset: 3"
If offsets are wrong, check _tls.core_offset assignment in _run_inference().
```

---

## Investigate AIPU configure_device blocking

```
A server process is hanging at startup (taking >30 seconds to respond to start commands).
Please:
1. Check if /tmp/.axelera_board_fw_loaded exists on the device
2. Check if AXELERA_CONFIGURE_BOARD is set in the process environment
3. Look at the server log for "configure_device blocked" or "AIPU cores READY" messages
4. If the flag is missing and CONFIGURE_BOARD is not 0, the first process is trying to 
   flash firmware — this is expected. If it's a second process, the flag mechanism failed.
Reference: rules.md "AXELERA_CONFIGURE_BOARD=0 Is Required for Process 2+"
```

---

## Reduce USB bandwidth for two cameras

```
Both cameras are on the same USB controller and one is failing with
"Not enough bandwidth for new device state" in dmesg.
Please modify _patched_build_gst_usb in axelera_server.py to default to
640x480 resolution when no dimensions are specified in the source config.
The change should only affect the capsfilter 'dimensions' fallback,
not break configs that explicitly specify width/height.
```

---

## View and tail a live server log

```
Please tail the most recent server log on the device.
Run: wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 'ls -lt /data/edgex-gui/logs/ | head -5'"
Then read the newest log file and show me the last 50 lines.
```

---

## Full reset and clean test

```
I want to do a clean test from scratch. Please:
1. Kill all running server processes on the device
2. Delete /tmp/.axelera_board_fw_loaded
3. Delete /tmp/srv_yolo.log and /tmp/srv_face.log
4. Sync the latest axelera_server.py to the device
5. Run /tmp/test_multiprocess.sh and show full output
```
