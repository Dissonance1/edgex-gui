#!/bin/bash
# Sync local source to Aetina for native build
TARGET="aetina@10.12.83.250"
REMOTE_DIR="/data/edgex-gui"

echo "--- Syncing EdgeX GUI Source to Aetina ---"
ssh $TARGET "mkdir -p $REMOTE_DIR"

# Sync src, resources, CMakeLists.txt and axelera_runtime
# Exclude build artifacts, venv, and large logs
rsync -avz --progress \
    --exclude 'build/' \
    --exclude '.git/' \
    --exclude '__pycache__/' \
    --exclude 'logs/' \
    --exclude 'venv/' \
    ./ $TARGET:$REMOTE_DIR/

echo "--- Sync Complete ---"
