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

# Profile 1: face
cfg1 = {
    'profileName': 'face',
    'modelPath': '/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml',
    'cameraSource': 'usb:21',
    'aipuCores': '0,1',
    'edgexDeviceName': '_Face-recog',
    'edgexProfileName': 'face_recog'
}

# Profile 2: FaceRecog
cfg2 = {
    'profileName': 'FaceRecog',
    'modelPath': '/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml',
    'cameraSource': 'usb:20',
    'aipuCores': '2,3',
    'edgexDeviceName': '_Face-recog',
    'edgexProfileName': 'face_recog'
}

print('Starting Profile face:', send_cmd('start:' + json.dumps(cfg1)))
time.sleep(5)
print('Starting Profile FaceRecog:', send_cmd('start:' + json.dumps(cfg2)))
time.sleep(5)

print('\n--- Current Status ---')
print(send_cmd('status'))

time.sleep(10)
print('\n--- Stopping both ---')
print('Stop face:', send_cmd('stop:face'))
print('Stop FaceRecog:', send_cmd('stop:FaceRecog'))
print('Final Status:', send_cmd('status'))
print('Done.')
