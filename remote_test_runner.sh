#!/bin/bash
# remote_test_runner.sh - Runs the test worker on Aetina device with all dependencies

# 1. Use existing virtual environment at /data/voyager-sdk/venv
echo "Activating SDK virtual environment..."
if [ -d "/data/voyager-sdk/venv" ]; then
    source /data/voyager-sdk/venv/bin/activate
else
    echo "Warning: /data/voyager-sdk/venv not found!"
fi

# 2. Configuration for Aetina
export AXELERA_FRAMEWORK="/data/voyager-sdk"
export AXELERA_RUNTIME_DIR="/opt/axelera/runtime-1.4.0-1"
export PYTHONPATH="$AXELERA_FRAMEWORK:$PYTHONPATH"

# 3. Library and GStreamer Plugin paths for Aetina
export LD_LIBRARY_PATH="/data/voyager-sdk/operators/lib:/opt/axelera/runtime-1.4.0-1/lib:/opt/axelera/runtime-1.2.5-1/lib:$LD_LIBRARY_PATH"
export GST_PLUGIN_PATH="/data/voyager-sdk/operators/lib:/opt/axelera/runtime-1.2.5-1/lib/gstreamer-1.0:$GST_PLUGIN_PATH"

# 4. Run the test
echo "Starting test_worker.py on Aetina with GST_DEBUG=3..."
export GST_DEBUG=3
python3 test_worker.py "$@" 2>&1 | tee test_output.log
