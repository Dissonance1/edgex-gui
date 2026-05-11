#!/bin/bash
# Test multi-process approach: one axelera_server.py per profile (matches GUI design)
cd /data/edgex-gui
source ./setup_axelera_env.sh > /dev/null 2>&1

PYBIN=/data/voyager-sdk/venv/bin/python3
SRV1_LOG=/tmp/srv_yolo.log
SRV2_LOG=/tmp/srv_face.log

# Kill any old servers
pkill -f "python3 axelera_server" 2>/dev/null || true
sleep 2
rm -f /tmp/.axelera_board_fw_loaded

echo "--- [1] Starting YOLO server (port 5567, cores 0-1) ---"
AXELERA_CORE_OFFSET=0 AXELERA_CORE_LIMIT=2 \
    $PYBIN axelera_server.py --cmd-port 5567 --video-port 5568 --meta-port 5566 > $SRV1_LOG 2>&1 &
SRV1=$!
sleep 8

echo "--- [2] Sending YOLO start ---"
printf 'start:{"modelPath":"yolov8n-coco-onnx","cameraSource":"usb:20","aipuCores":"0,1","confidenceThreshold":0.5}\n' \
    | nc -q2 127.0.0.1 5567 | cat
sleep 15

echo "--- [3] Starting FACE server (port 5570, cores 2-3) ---"
AXELERA_CORE_OFFSET=2 AXELERA_CORE_LIMIT=2 \
    $PYBIN axelera_server.py --cmd-port 5570 --video-port 5571 --meta-port 5569 > $SRV2_LOG 2>&1 &
SRV2=$!
sleep 5

echo "--- [4] Sending FACE start ---"
printf 'start:{"modelPath":"/data/voyager-sdk/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml","cameraSource":"usb:22","aipuCores":"2,3","confidenceThreshold":0.4}\n' \
    | nc -q2 127.0.0.1 5570 | cat
sleep 20

echo "--- [5] Checking YOLO status ---"
echo 'status' | nc -q1 127.0.0.1 5567 | cat

echo "--- [6] Checking FACE status ---"
echo 'status' | nc -q1 127.0.0.1 5570 | cat

echo "--- [7] Stopping ---"
echo 'shutdown' | nc -q1 127.0.0.1 5567 | cat
echo 'shutdown' | nc -q1 127.0.0.1 5570 | cat
wait $SRV1 $SRV2 2>/dev/null

echo "=== YOLO SERVER LOG ==="
cat $SRV1_LOG

echo ""
echo "=== FACE SERVER LOG ==="
cat $SRV2_LOG
