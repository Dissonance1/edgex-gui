#!/bin/bash
cd /data/edgex-gui
source ./setup_axelera_env.sh > /dev/null 2>&1
pkill -f "python3 axelera_server" 2>/dev/null
sleep 1
/data/voyager-sdk/venv/bin/python3 axelera_server.py --log-file /tmp/srv_iomode.log &
sleep 10
cat /tmp/srv_iomode.log
