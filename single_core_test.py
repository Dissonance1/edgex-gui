import socket
import json
import time

def send_cmd(cmd):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(('127.0.0.1', 5567))
        sock.sendall(cmd.encode())
        res = sock.recv(1024).decode()
        sock.close()
        return res
    except Exception as e:
        return str(e)

# Profile 1: face (Core 0)
cfg1 = {
    'profileName': 'face',
    'modelPath': '/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml',
    'cameraSource': 'usb:21',
    'aipuCores': '0',
    'edgexDeviceName': '_Face-recog',
    'edgexProfileName': 'face_recog'
}

# Profile 2: FaceRecog (Core 1)
cfg2 = {
    'profileName': 'FaceRecog',
    'modelPath': '/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml',
    'cameraSource': 'usb:20',
    'aipuCores': '1',
    'edgexDeviceName': '_Face-recog',
    'edgexProfileName': 'face_recog'
}

print('Starting face-core0:', send_cmd('start:' + json.dumps(cfg1)))
time.sleep(20)
print('Starting FaceRecog-core1:', send_cmd('start:' + json.dumps(cfg2)))
time.sleep(5)
print('Status:', send_cmd('status'))
time.sleep(5)
print('Stop both:', send_cmd('stop:face'), send_cmd('stop:FaceRecog'))
