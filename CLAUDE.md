# EdgeX-GUI — Claude Code Configuration

## Project Summary
Multi-camera AI inference GUI for the **Axelera Metis AIPU** on an **Aetina AN110 (aarch64)** board.
The Qt C++ GUI spawns one `axelera_server.py` process per camera profile; each process runs one
GStreamer-based inference pipeline on a dedicated AIPU core partition and streams results back to the GUI.

## Device Access
```
Device : aetina@10.12.83.250
Deploy : cd /home/rog_strix/src/edgex-gui && bash sync_to_aetina.sh
SSH    : wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 '<cmd>'"
Rsync  : wsl -d Ubuntu bash -c "rsync -avz --exclude '__pycache__' -e 'ssh -o StrictHostKeyChecking=no' <file> aetina@10.12.83.250:/data/edgex-gui/"
Remote path: /data/edgex-gui/
SDK path   : /data/voyager-sdk/
Venv python: /data/voyager-sdk/venv/bin/python3
```

## Key Files
| File | Purpose |
|------|---------|
| `axelera_server.py` | Main inference server — most-edited file |
| `src/AIInferenceWorker.cpp` | Qt GUI worker that spawns server processes |
| `config_multi_camera.json` | Multi-stream config for testing |
| `setup_axelera_env.sh` | Sets PYTHONPATH, LD_LIBRARY_PATH, GST_PLUGIN_PATH |
| `start_backend.sh` | Wrapper that sources env then execs the server |
| `sync_to_aetina.sh` | Rsync key files to device |
| `test_multiprocess.sh` | Two-process two-camera integration test |

## Environment Setup (on device)
```bash
cd /data/edgex-gui
source ./setup_axelera_env.sh   # must be done before running server
/data/voyager-sdk/venv/bin/python3 axelera_server.py [--cmd-port 5567] [--log-file /tmp/srv.log]
```
> **CRLF warning**: `setup_axelera_env.sh` and `start_backend.sh` must have Unix line endings.
> If synced from Windows, run `dos2unix` on the device: `dos2unix /data/edgex-gui/setup_axelera_env.sh /data/edgex-gui/start_backend.sh`

## Server Command Protocol
```
start:{json}\n                      # GUI single-profile format (profile name = "default")
start:ProfileName:{json}\n          # named multi-profile format
stop:ProfileName\n                  # stop one profile
stop\n                              # stop all profiles
status\n                            # returns JSON status
shutdown\n                          # graceful exit
```

## Testing on Device
```bash
# Quick two-process test (proper architecture):
bash /tmp/test_multiprocess.sh

# Check server log:
cat /tmp/srv_yolo.log
cat /tmp/srv_face.log

# Send a command manually:
printf 'start:{"modelPath":"yolov8n-coco-onnx","cameraSource":"usb:20","aipuCores":"0,1","confidenceThreshold":0.5}\n' \
    | nc -q2 127.0.0.1 5567

# Check status:
echo 'status' | nc -q1 127.0.0.1 5567
```

## Port Assignments (GUI multi-profile)
| Profile | CMD | VIDEO | META |
|---------|-----|-------|------|
| Profile 1 | 5567 | 5568 | 5566 |
| Profile 2 | 5570 | 5571 | 5569 |
| Profile 3 | 5573 | 5574 | 5572 |

## Common Commands
```bash
# Sync server only (fast iteration):
wsl -d Ubuntu bash -c "rsync -avz --exclude '__pycache__' -e 'ssh -o StrictHostKeyChecking=no' /home/rog_strix/src/edgex-gui/axelera_server.py aetina@10.12.83.250:/data/edgex-gui/"

# Kill all running servers on device:
wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 'pkill -f axelera_server || true'"

# Clear AIPU boot flag (force re-initialisation on next start):
wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 'rm -f /tmp/.axelera_board_fw_loaded'"

# View recent logs:
wsl -d Ubuntu bash -c "ssh -o StrictHostKeyChecking=no aetina@10.12.83.250 'ls -lt /data/edgex-gui/logs/ | head -10'"
```
