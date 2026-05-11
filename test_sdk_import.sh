#!/bin/bash
cd /data/edgex-gui
source ./setup_axelera_env.sh > /dev/null 2>&1
echo "PYTHONPATH=$PYTHONPATH"
echo "Testing import..."
/data/voyager-sdk/venv/bin/python3 -c "
import sys
sys.path.insert(0, '/data/voyager-sdk')
print('sys.path:', [p for p in sys.path if 'axelera' in p or 'voyager' in p])
try:
    import axelera.app.device_manager
    print('IMPORT OK')
except Exception as e:
    print('IMPORT FAIL:', e)
"
