# Axelera Video Stream Debugging Guide

This document provides a systematic approach to debugging the "Inference OK, but No Video" symptom in the Axelera Voyager SDK + GStreamer + SHM architecture.

## 1. Technical Architecture Overview

The system uses a **Producer-Consumer** model:
- **Producer (`axelera_server.py`)**:
    - Runs the Axelera inference stream.
    - Captures frames from the camera.
    - Pushes raw BGR frames into a GStreamer pipeline starting with `appsrc`.
    - Streams metadata via a TCP server on port 5566.
    - Streams video frames via `shmsink` to `/tmp/ax_bridge/shm.sock`.
- **Consumer (GUI Application)**:
    - Connects to the TCP server on 5566 as a client to get detections.
    - Sends commands (start/stop) to port 5567.
    - Connects to `/tmp/ax_bridge/shm.sock` via `shmsrc` to ingest video frames.

---

## 2. Debugging Checklist

### A. Verify GStreamer Producer (Backend)
The most common failure point is that the `shmsink` element is not actually listening or producing buffers.

1.  **Check Socket Existence and Type**:
    ```bash
    ls -la /tmp/ax_bridge/shm.sock
    ```
    - It should start with `s` (socket). If it's a regular file or missing, the pipeline failed.
2.  **Check Producer Logs**:
    - Look for "SHM Video streamer activated".
    - Check for "SHM Push error". If detected, the `appsrc` isn't getting frames from Python.
3.  **Independent Producer Test**:
    Stop the backend and run a dummy producer to see if the GUI can see it:
    ```bash
    gst-launch-1.0 videotestsrc is-live=true ! video/x-raw,width=640,height=480,framerate=30/1 ! videoconvert ! video/x-raw,format=I420 ! shmsink socket-path=/tmp/ax_bridge/shm.sock shm-size=100000000
    ```

### B. Verify GStreamer Consumer (GUI/CLI)
If the producer is running, test if a standard GStreamer client can see it.

1.  **Independent Consumer Test**:
    ```bash
    gst-launch-1.0 shmsrc socket-path=/tmp/ax_bridge/shm.sock ! decodebin ! videoconvert ! autovideosink
    ```
    - If this fails with **"Connection refused"**, the `shmsink` is likely NOT in the `PLAYING` state or is bound to the wrong path.
    - If this shows a video but the GUI doesn't, the issue is in the C++ `appsink` or rendering loop.

### C. Permissions (The "Root vs User" Gotcha)
Since the GUI runs as `root` and the daemon as `aetina`:
1.  **Directory Permissions**: `/tmp/ax_bridge` must be executable/searchable by both.
    - `sudo chmod 777 /tmp/ax_bridge`
2.  **Socket Permissions**: `shmsink` creates the socket.
    - The Python script must call `os.chmod(shm_socket, 0o777)` *after* the pipeline goes to `PLAYING`.
3.  **Shared Memory Permissions**: GStreamer SHM uses POSIX shared memory.
    - Ensure `aetina` and `root` can both access the memory segments. If still stuck, try running the daemon as `sudo`.

### D. Network & Ports
1.  **Telemetry (5566)**:
    - Use `ss -lnpt | grep 5566` on the Aetina. It should show a listener on `0.0.0.0:5566`.
    - Use `telnet 127.0.0.1 5566` to see if a connection is accepted.
2.  **GUI Connection**:
    - If the GUI is running on the *same* machine as the backend, use `127.0.0.1`.
    - If the GUI is running on a different machine (e.g., your laptop), ensure the backend `bind` is to `0.0.0.0` and not `localhost`. Note that **SHM video will not work over the network** (requires local access).

---

## 3. Recommended Clean Architecture

To ensure reliability and automatic recovery, follow this pattern:

### 1. Telemetry Server (Port 5566)
- **Pattern**: Server (Backend) / Client (GUI).
- **Reason**: The backend lives longer than single GUI instances.
- **Protocol**: 
    - 4-byte total length (le)
    - 4-byte metadata length (le)
    - 4-byte flags
    - JSON payload

### 2. Command Server (Port 5567)
- **Pattern**: Request/Response (TCP).
- **Commands**: `start`, `stop`, `status`.

### 3. SHM Video Bridge
- **Pattern**: `shmsink` (Backend) / `shmsrc` (GUI).
- **Critical Settings**:
    - `shm-size`: Target `100,000,000` bytes (100MB) for 1080p/4k headroom.
    - `wait-for-connection=false`: Ensures the backend doesn't freeze if the GUI isn't running.
    - `sync=false`: Prevents pipeline stalling if frames are pushed faster than consumed.

---

## 4. Troubleshooting "Black Screen" Gotchas

| Symptom | Probable Cause | Fix |
| :--- | :--- | :--- |
| **Connection Refused** | `shmsink` isn't in `PLAYING` state. | Ensure `gst_pipeline.set_state(Gst.State.PLAYING)` is called and successful in Python. |
| **Broken Pipe** | Stale socket file in `/tmp`. | `os.remove(shm_socket)` before starting the pipeline. |
| **Black/Laggy Frames** | Caps mismatch (e.g. BGR vs I420). | Use `videoconvert` explicitly in both sink and src pipelines. |
| **No Detections** | Telemetry server roles swapped. | Ensure Backend is **Listening** and GUI is **Connecting**. |
| **Permission Denied** | Root (GUI) can't read `aetina` socket. | `os.chmod` the socket to 777 *after* the producer starts. |

---

## 5. Diagnostic Commands Summary

```bash
# Check if ports are listening
sudo ss -lnpt | grep -E '5566|5567'

# Check socket permissions
ls -la /tmp/ax_bridge/shm.sock

# Watch frame counters in backend log
tail -f /tmp/axelera_server.logs

# Test video ingestion manually
gst-launch-1.0 shmsrc socket-path=/tmp/ax_bridge/shm.sock ! decodebin ! videoconvert ! autovideosink
```
