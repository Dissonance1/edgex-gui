import socket, json, sys

def send_cmd(cmd):
    s = socket.socket()
    s.connect(("127.0.0.1", 5567))
    s.sendall(cmd.encode())
    resp = s.recv(4096).decode()
    s.close()
    return resp

XRAY_CFG = {
    "modelPath":           "/data/voyager-sdk/customers/xray/yolov8n-xray-baggage.yaml",
    "cameraSource":        "usb:20", # Changed from usb:0 to 20 for Brio
    "aipuCores":           "1",      # Singular as requested
    "confidenceThreshold": 0.4,
    "pipelineType":        "gst",
    "displayMode":         "none",
    "edgexDeviceName":     "Brio_100",
    "edgexProfileName":    "Camera_Profile",
    "edgexBn":             "xray-01"
}

print(send_cmd(f"start:Xray:{json.dumps(XRAY_CFG)}"))
