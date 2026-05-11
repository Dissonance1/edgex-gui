#!/bin/bash
# Test two cameras simultaneously with the new io-mode=mmap patch
cd /data/edgex-gui
source ./setup_axelera_env.sh > /dev/null 2>&1

SRV_LOG=/tmp/srv_two_cam.log

# Kill any old server
pkill -f "python3 axelera_server" 2>/dev/null || true
sleep 2

# Clear log flag so first process initialises the board fresh
rm -f /tmp/.axelera_board_fw_loaded

echo "--- Starting server ---"
/data/voyager-sdk/venv/bin/python3 axelera_server.py > $SRV_LOG 2>&1 &
SRV_PID=$!
sleep 5

echo "--- Sending YOLO start (usb:20, cores 0,1) ---"
echo 'start:YOLO:{"modelPath":"yolov8n-coco-onnx","cameraSource":"usb:20","aipuCores":"0,1","confidenceThreshold":0.5}' \
    | nc -q1 127.0.0.1 5567 | cat
sleep 10

echo "--- Sending FACE start (usb:22, cores 2,3) ---"
echo 'start:FACE:{"modelPath":"/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml","cameraSource":"usb:22","aipuCores":"2,3","confidenceThreshold":0.4}' \
    | nc -q1 127.0.0.1 5567 | cat
sleep 15

echo "--- Status ---"
echo 'status' | nc -q1 127.0.0.1 5567 | cat

echo "--- Stopping ---"
echo 'stop' | nc -q1 127.0.0.1 5567 | cat
sleep 2
kill $SRV_PID 2>/dev/null

echo "=== SERVER LOG ==="
cat $SRV_LOG
