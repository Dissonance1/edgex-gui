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

# Start Profile 1
cfg1 = {'profileName': 'Prof1', 'edgexDeviceName': 'Dev1', 'modelPath': 'yolov8n-coco-onnx', 'cameraSource': 'usb:20'}
print('Starting Prof1:', send_cmd('start:' + json.dumps(cfg1)))
time.sleep(2)

# Start Profile 2
cfg2 = {'profileName': 'Prof2', 'edgexDeviceName': 'Dev2', 'modelPath': 'yolov8n-coco-onnx', 'cameraSource': 'usb:20'}
print('Starting Prof2:', send_cmd('start:' + json.dumps(cfg2)))
time.sleep(2)

# Check status
print('Status:', send_cmd('status'))

# Check preview switch
print('Switching preview to Prof1:', send_cmd('preview:Prof1'))
time.sleep(1)
print('Switching preview to Prof2:', send_cmd('preview:Prof2'))

# Stop Prof1
print('Stopping Prof1:', send_cmd('stop:Prof1'))
time.sleep(1)

# Final status
print('Final Status:', send_cmd('status'))
print('Done.')
