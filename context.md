# Project Context — EdgeX-GUI

## What It Is
A real-time AI inference system for edge deployment on the **Axelera Metis AIPU**.
- **GUI**: C++ Qt application (`src/`) — lets the user create and manage camera profiles
- **Backend**: Python `axelera_server.py` — one process per profile, handles inference + EdgeX publishing
- **Hardware**: Aetina AN110 board, ARM aarch64, 4 AIPU cores (0–3), USB cameras

## How It Runs
```
Qt GUI (AIInferenceWorker.cpp)
    │  spawns one process per profile
    ▼
axelera_server.py  ──────────────────────────────────────────────
  │  CMD port (5567/5570/5573)  ← receives start/stop commands
  │  VIDEO port (5568/5571/5574) → streams JPEG frames to GUI
  │  META port (5566/5569/5572)  → streams telemetry JSON to GUI
  │
  ├─ _run_inference() thread
  │     │  creates GStreamer pipeline via Axelera SDK
  │     │  v4l2src → jpegdec → axinferencenet → appsink
  │     │  each frame: encode JPEG + extract detections
  │     └─ sends results to EdgeX via HTTP
  │
  └─ axelera SDK (Python) → axruntime (C library) → AIPU driver
```

## AIPU Core Architecture
The Metis chip has **4 inference cores (0–3)**. Each server process owns a partition:
- Process 1: cores 0,1 (env: `AXELERA_CORE_OFFSET=0 AXELERA_CORE_LIMIT=2`)
- Process 2: cores 2,3 (env: `AXELERA_CORE_OFFSET=2 AXELERA_CORE_LIMIT=2`)

The core offset is applied via a **monkey-patch** on `device_manager._AipuDeviceManager._configure_boards`
because the SDK hardcodes core_index=0 per process.

## SDK Location
```
/data/voyager-sdk/
├── axelera/app/          # Python SDK
│   ├── device_manager.py # AIPU board config — patched for multi-process
│   ├── pipe/io.py        # GStreamer pipeline builder — patched for io-mode
│   ├── pipe/gst_helper.py# GStreamer element creation
│   └── stream.py         # create_inference_stream()
├── ax_models/            # Pre-compiled model ZOOs
├── build/                # Compiled model binaries
├── customers/xray/       # X-ray baggage model
├── embeddings.json       # Face recognition embeddings
└── venv/                 # Python virtual environment
```

## Camera Setup
| Camera | Device Nodes | USB Bus |
|--------|-------------|---------|
| Brio 100 #1 (YOLO/Xray) | /dev/video20 (capture), /dev/video21 (metadata) | usb 7-1.1 |
| Brio 100 #2 (Face) | /dev/video22 (capture), /dev/video23 (metadata) | usb 7-1.2 |

> **Important**: `usb:20` means `/dev/video20`. `usb:21` is a METADATA node (no video capture).
> Always use even-numbered nodes for capture (20, 22, 24...).

## EdgeX Integration
The server publishes inference results to EdgeX Foundry:
- URL: `http://localhost:4000/core-data/api/v3/event/device-rest/{device}/{profile}/all`
- Format: SenML JSON payload
- The `_profile_cache` dict stores fetched device profiles per profile name
- Profile resources are auto-resolved to inference output fields via token matching

## Models Available
| Model | Path | Task |
|-------|------|------|
| yolov8n-coco-onnx | ax_models/zoo/yolo/object_detection/yolov8n-coco-onnx.yaml | 80-class COCO detection |
| voyager-person-detection | ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml | Person + face recognition |
| yolov8n-xray-baggage | customers/xray/yolov8n-xray-baggage.yaml | X-ray baggage detection |

## Known Hardware Constraints
1. **USB bandwidth**: Both Brio 100 cameras share USB controller `usb 7`. Running both at default
   resolution (1080p MJPEG) exceeds bandwidth. Fix: reduce to 640×480 or use different USB ports.
2. **AIPU cores**: 4 cores total. Face recognition model uses 2 tasks (detect on core 2,
   recognise on core 3). YOLO uses cores 0–1. No cores left for a third concurrent model.
3. **Single stream per process**: The SDK's `create_inference_stream()` is NOT safe to call
   concurrently in the same process. Each profile MUST run in its own process.

## Boot/Init Sequence
1. First server process: `configure_device()` runs → flashes firmware → creates `/tmp/.axelera_board_fw_loaded`
2. Every subsequent process: sees the flag → sets `AXELERA_CONFIGURE_BOARD=0` → SDK skips board config entirely
3. Flag is cleared on reboot; manually: `rm -f /tmp/.axelera_board_fw_loaded`
