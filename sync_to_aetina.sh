#!/bin/bash

# Configuration
TARGET_USER="aetina"
TARGET_IP="10.12.83.250"
TARGET_DIR="/data/edgex-gui"

# Local directories to sync
FILES_TO_SYNC=(
    "src"
    "scripts"
    "axelera_server.py"
    "smoke_test.py"
    "config_multi_camera.json"
    "test_single.json"
    "CMakeLists.txt"
    "setup_axelera_env.sh"
)

echo "--- Cleaning up legacy files on Aetina ($TARGET_IP) ---"
ssh -o StrictHostKeyChecking=no $TARGET_USER@$TARGET_IP "rm -rf $TARGET_DIR/axelera_runtime $TARGET_DIR/run_runtime.py $TARGET_DIR/launch_debug.py $TARGET_DIR/test_worker.py"

echo "--- Syncing EdgeX-GUI to Aetina ($TARGET_IP) ---"

for item in "${FILES_TO_SYNC[@]}"; do
    echo "Syncing $item..."
    rsync -avz --exclude "__pycache__" --exclude "*.o" --exclude "build" \
        -e "ssh -o StrictHostKeyChecking=no" \
        "$item" "$TARGET_USER@$TARGET_IP:$TARGET_DIR/"
done

echo "--- Sync Complete ---"
echo "To test on Aetina:"
echo "1. SSH into the device: ssh $TARGET_USER@$TARGET_IP"
echo "2. Run real inference: python3 axelera_server.py --config config_multi_camera.json"
