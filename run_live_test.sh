#!/bin/bash
# Kill any existing backend, start fresh, run live video test
pkill -f axelera_server 2>/dev/null || true
sleep 3
cd /data/edgex-gui
nohup bash start_backend.sh > /tmp/backend.log 2>&1 &
BGPID=$!
echo "Backend PID: $BGPID"
sleep 6
echo "--- Backend startup log ---"
cat /tmp/backend.log
echo "--- Running live video test ---"
python3 live_test.py
echo "--- Backend log (after test) ---"
cat /tmp/backend.log
