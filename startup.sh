#!/bin/bash

###############################################################
# Aetina System Clean Restart Script
# Usage: ./startup.sh
###############################################################

EDGEX_DIR="/data/edgex"
GUI_DIR="/data/edgex-gui"

echo "================================================"
echo "  Aetina System Clean Restart"
echo "================================================"

# --- Step 0: Kill everything by name ---
echo "[1/3] Killing existing processes..."
sudo pkill -9 -f axelera_server || true
sudo pkill -9 -f edgex-qt-ui    || true
sudo pkill -9 -f core-metadata  || true
sudo pkill -9 -f core-data      || true
sudo pkill -9 -f core-command   || true
sudo pkill -9 -f support-notifications || true
sudo pkill -9 -f support-scheduler    || true
sudo pkill -9 -f app-new-service      || true
sudo pkill -9 -f device-rest          || true
sudo pkill -9 -f kuiperd              || true
sudo pkill -9 -f consul               || true
sudo pkill -9 -f redis-server         || true

# Wait for all ports to be released (TIME_WAIT etc.)
echo "    Waiting for ports to release..."
sleep 6

# --- Step 1: Start EdgeX ---
echo ""
echo "[2/3] Starting EdgeX services..."
if [ -f "$EDGEX_DIR/start-edgex.sh" ]; then
    cd "$EDGEX_DIR"
    bash start-edgex.sh
    echo "    EdgeX started."
else
    echo "    ERROR: $EDGEX_DIR/start-edgex.sh not found."
fi

# Wait for EdgeX services to settle before starting backend
sleep 3

# --- Step 2: Start Axelera Inference Backend ---
echo ""
echo "[3/3] Starting Axelera Inference Backend..."
if [ -f "$GUI_DIR/start_backend.sh" ]; then
    cd "$GUI_DIR"
    bash start_backend.sh
    echo "    Backend started (logs: /tmp/axelera_server.log)"
else
    echo "    ERROR: $GUI_DIR/start_backend.sh not found."
fi

# Verify backend ports
sleep 2
echo ""
echo "  Checking backend ports..."
if sudo netstat -tuln | grep -q ':5567'; then
    echo "  ✓ Port 5567 (Command) is UP"
else
    echo "  ✗ Port 5567 not up — trying again..."
    cd "$GUI_DIR"
    bash start_backend.sh
    sleep 2
fi

echo ""
echo "================================================"
echo "  All services restarted!"
echo ""
echo "  1. Restart the GUI."
echo "  2. Click 'Launch' to start inference."
echo "  Logs: tail -f /tmp/axelera_server.log"
echo "================================================"
