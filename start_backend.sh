#!/bin/bash

# Navigate to the script directory
cd "$(dirname "$0")"

# Source the Axelera environment
if [ -f "./setup_axelera_env.sh" ]; then
    echo "Sourcing ./setup_axelera_env.sh"
    source ./setup_axelera_env.sh
else
    echo "Warning: ./setup_axelera_env.sh not found."
fi

# Launch the Axelera Server
echo "Launching axelera_server.py using venv..."
# Use python3 from venv explicitly
exec "${VIRTUAL_ENV}/bin/python3" -u axelera_server.py "$@"
