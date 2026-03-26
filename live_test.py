#!/usr/bin/env python3
"""
Live camera test using the actual saved profile settings.
Tests the exact path that the GUI uses when launching inference.
Must be run AFTER start_backend.sh is active.
"""
import socket, json, time, struct, threading

SERVER_HOST = "127.0.0.1"
CMD_PORT    = 5567

def send_cmd(cmd, timeout=5):
    try:
        s = socket.socket()
        s.settimeout(timeout)
        s.connect((SERVER_HOST, CMD_PORT))
        s.sendall(cmd.encode())
        resp = s.recv(4096).decode().strip()
        s.close()
        return resp
    except Exception as e:
        return f"ERROR: {e}"

# Exact profile from QSettings
FACE_CFG = {
    "modelPath":           "/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml",
    "cameraSource":        "usb:20",  # /dev/video20 = Brio 100 capture node
    "aipuCores":           "4",
    "confidenceThreshold": 0.5,
    "pipelineType":        "gst",
    "displayMode":         "none",
    "edgexDeviceName":     "_Face-recog",
    "edgexProfileName":    "face_recog",
    "edgexBn":             "face-01",
    "classMapPath":        "",
    "embeddingPath":       ""
}

print("="*60)
print("LIVE MODEL TEST (face profile, gst pipeline)")
print("="*60)

resp = send_cmd(f"start:face:{json.dumps(FACE_CFG)}", timeout=10)
print(f"  Start response: {resp}")

if not resp.startswith("OK"):
    print("  FAILED to start stream. Is backend running?")
    raise SystemExit(1)

# Parse video port from "OK face@PORT"
try:
    video_port = int(resp.split("@")[1])
except Exception:
    video_port = 5568
print(f"  Video port: {video_port}")

# Wait for model init
print("  Waiting 20s for model to initialize...")
time.sleep(20)

# Try to receive one video frame
print(f"  Connecting to video port {video_port}...")
try:
    vs = socket.socket()
    vs.settimeout(10)
    vs.connect((SERVER_HOST, video_port))

    # Read 4-byte big-endian frame length header
    header = b""
    while len(header) < 4:
        chunk = vs.recv(4 - len(header))
        if not chunk:
            raise RuntimeError("Connection closed")
        header += chunk

    frame_len = struct.unpack(">I", header)[0]
    print(f"  Frame length: {frame_len} bytes")

    if 0 < frame_len < 5_000_000:
        data = b""
        while len(data) < frame_len:
            chunk = vs.recv(frame_len - len(data))
            if not chunk:
                break
            data += chunk
        print(f"  ✅ Received {len(data)} bytes of JPEG video!")
    else:
        print(f"  ❌ Invalid frame length: {frame_len}")
    vs.close()
except Exception as e:
    print(f"  ❌ Video connection failed: {e}")

print("\n  Stopping face stream...")
print(f"  Stop response: {send_cmd('stop:face')}")
