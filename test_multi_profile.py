#!/usr/bin/env python3
"""
Quick integration test: starts the server, sends a start command with
a real saved profile config, waits for it to bring the stream online or
fail, then prints all captured logs.
"""
import os, sys, time, socket, json, subprocess, threading, signal

sdk_root = os.environ.get("AXELERA_FRAMEWORK", "/data/voyager-sdk")
SERVER_PY = "/data/edgex-gui/axelera_server.py"

# ── Saved profile from QSettings ──────────────────────────────────────────────
# Pull the QSettings .ini to find what profile the GUI actually saved
import configparser, glob

SETTINGS_CANDIDATES = glob.glob("/home/aetina/.config/*/*.ini") + \
                      glob.glob("/home/aetina/.config/*.ini")

profile_cfg = None
for f in SETTINGS_CANDIDATES:
    try:
        cp = configparser.ConfigParser()
        cp.read(f, encoding="utf-8")
        # QSettings uses arrays like profiles\1\name
        for section in cp.sections():
            keys = list(cp[section].keys())
            if any("name" in k for k in keys):
                print(f"[SETTINGS] Found potential profile in {f}, section [{section}]")
        print(f"[SETTINGS] File: {f}")
        print(dict(cp.items(cp.sections()[0] if cp.sections() else "DEFAULT")))
    except Exception as e:
        print(f"[SETTINGS WARN] {f}: {e}")

# ── Fallback: use hardcoded Xray profile ─────────────────────────────────────
XRAY_CFG = {
    "modelPath":           "/data/voyager-sdk/customers/xray/yolov8n-xray-baggage.yaml",
    "cameraSource":        "usb:0",
    "aipuCores":           "0,1",
    "confidenceThreshold": 0.4,
    "pipelineType":        "gst",
    "displayMode":         "none",
    "edgexDeviceName":     "Brio_100",
    "edgexProfileName":    "Camera_Profile",
    "edgexBn":             "xray-01",
    "classMapPath":        "",
    "embeddingPath":       ""
}

FACE_CFG = {
    "modelPath":           "/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml",
    "cameraSource":        "usb:0",
    "aipuCores":           "2,3",
    "confidenceThreshold": 0.4,
    "pipelineType":        "gst",
    "displayMode":         "none",
    "edgexDeviceName":     "Brio_100",
    "edgexProfileName":    "Camera_Profile",
    "edgexBn":             "face-01",
    "classMapPath":        "",
    "embeddingPath":       ""
}

LOGS = []

def send_cmd(cmd, timeout=5):
    try:
        s = socket.socket()
        s.settimeout(timeout)
        s.connect(("127.0.0.1", 5567))
        s.sendall(cmd.encode())
        resp = s.recv(4096).decode("utf-8", errors="replace").strip()
        s.close()
        return resp
    except Exception as e:
        return f"ERROR: {e}"

print("=" * 70)
print("INTEGRATION TEST: Multi-Profile Inference")
print("=" * 70)

# ── Start server ──────────────────────────────────────────────────────────────
print("\n[1] Starting axelera_server.py ...")
srv = subprocess.Popen(
    [sys.executable, SERVER_PY],
    stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
    text=True, bufsize=1,
    env={**os.environ, "AXELERA_FRAMEWORK": sdk_root}
)

output_lines = []
def reader():
    for line in srv.stdout:
        output_lines.append(line.rstrip())
        print(f"  [SRV] {line.rstrip()}")

t = threading.Thread(target=reader, daemon=True)
t.start()
time.sleep(3)  # let server boot

# ── Xray start ───────────────────────────────────────────────────────────────
print("\n[2] Sending START for Xray profile ...")
cfg_json = json.dumps(XRAY_CFG)
resp = send_cmd(f"start:Xray:{cfg_json}")
print(f"  Server response: {resp}")

if "OK" in resp:
    print("  ✅ Xray stream started")
    time.sleep(10)  # let it stabilize

    # ── Face start ───────────────────────────────────────────────────────────
    print("\n[3] Sending START for Face profile ...")
    face_json = json.dumps(FACE_CFG)
    resp2 = send_cmd(f"start:Face:{face_json}", timeout=10)
    print(f"  Server response: {resp2}")
    
    if "OK" in resp2:
        print("  ✅ Face stream accepted — waiting 15s for initialization ...")
        time.sleep(15)
    else:
        print("  ❌ Face stream FAILED")
else:
    print("  ❌ Xray stream failed — check server logs below")
    time.sleep(5)

# ── Stop and collect logs ─────────────────────────────────────────────────────
print("\n[4] Stopping ...")
send_cmd("stop:Xray")
send_cmd("stop:Face")
send_cmd("shutdown")
time.sleep(2)
srv.terminate()
srv.wait(timeout=5)

print("\n" + "=" * 70)
print("FULL SERVER LOG:")
print("=" * 70)
for line in output_lines:
    print(line)
