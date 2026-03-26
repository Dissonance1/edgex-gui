#!/bin/bash
cd /data/edgex-gui
source /data/voyager-sdk/venv/bin/activate
export AXELERA_FRAMEWORK=/data/voyager-sdk
export AXELERA_RUNTIME_DIR=/opt/axelera/runtime-1.4.0-1
export LD_LIBRARY_PATH=/opt/axelera/runtime-1.4.0-1/lib:/data/voyager-sdk/operators/lib:$LD_LIBRARY_PATH

gdb -ex r -ex "bt" -ex "quit" --args python3 test_worker.py
