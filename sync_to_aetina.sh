#!/bin/bash

# Configuration
REMOTE_USER="aetina"
REMOTE_HOST="10.12.83.250"
REMOTE_PATH="/data/edgex-gui"

echo "Syncing RulesEngineView UI updates to Aetina device ($REMOTE_HOST)..."
echo "Target path: $REMOTE_PATH"

# Sync src directory and CMakeLists.txt
rsync -avz --progress \
    --exclude="build/" \
    --exclude="build-arm64/" \
    --exclude=".git/" \
    --exclude=".qtcreator/" \
    ./ ${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_PATH}/

echo "--------------------------------------------------------"
echo "Sync complete."
echo "Build commands on Aetina:"
echo "  cd $REMOTE_PATH/build && cmake .. && make -j4"
echo "--------------------------------------------------------"
