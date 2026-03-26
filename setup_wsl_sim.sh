#!/bin/bash

# Setup script for WSL Simulation Mode for EdgeX-GUI

echo "Setting up WSL Simulation environment..."

# 1. Base Paths (Adjust if your WSL paths are different)
export AXELERA_SDK_PATH="/home/rog_strix/voyager-sdk-mock" # Mock SDK path
export AXELERA_APP_PATH=$(pwd)
export AXELERA_RUNTIME_DIR="/opt/axelera/runtime-1.4.0-1" # Can be a mock or empty if in simulation mode

# 2. Simulation Toggle
export AXELERA_SIMULATION=1

# 3. GUI Connectivity
export AXELERA_GUI_HOST="127.0.0.1"
export AXELERA_RTSP_HOST="127.0.0.1"

# 4. Create Mock SDK directories if they don't exist
mkdir -p $AXELERA_SDK_PATH/ax_models/zoo/yolo/object_detection
mkdir -p $AXELERA_SDK_PATH/data
mkdir -p $AXELERA_SDK_PATH/build
mkdir -p $AXELERA_SDK_PATH/venv/bin

# 5. Create a fake model yaml for the UI to find
cat <<EOF > $AXELERA_SDK_PATH/ax_models/zoo/yolo/object_detection/voyager-person-detection.yaml
name: voyager-person-detection
description: "Simulated Person Detection Model for WSL testing"
version: 1.0
EOF

# 6. Inform the user
echo "------------------------------------------------"
echo "Environment configured for SIMULATION MODE."
echo "AXELERA_SIMULATION: $AXELERA_SIMULATION"
echo "AXELERA_SDK_PATH  : $AXELERA_SDK_PATH"
echo "AXELERA_GUI_HOST  : $AXELERA_GUI_HOST"
echo "------------------------------------------------"
echo "Run the GUI with: export DISPLAY=:0 && ./build/bin/edgex-qt-ui"
