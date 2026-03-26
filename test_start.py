import socket
import json
import time

def test():
    c = {
        "cameraSource": "usb:20",
        "modelPath": "voyager-person-detection",
        "confidenceThreshold": 0.5,
        "aipuCores": "4",
        "edgexDeviceName": "_Face-recog",
        "edgexProfileName": "face_recog",
        "edgexBn": "FaceDetector_2cf7f12052608e69_"
    }

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(5.0)
        s.connect(("127.0.0.1", 5567))
        cmd = f"start:{json.dumps(c)}"
        s.sendall(cmd.encode())
        resp = s.recv(1024).decode()
        print(f"Response: {resp}")
        s.close()
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    test()
