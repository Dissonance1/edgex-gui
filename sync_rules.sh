#!/bin/bash
# sync_rules.sh - Sync eKuiper streams and rules to remote Aetina

REMOTE_HOST="10.12.83.250"
API="http://localhost:9081"

echo "Updating eKuiper configuration on $REMOTE_HOST..."

# Function to update stream/rule via SSH using curl on the remote host
update_remote() {
    local type=$1    # streams or rules
    local name=$2
    local file=$3
    
    echo "Updating $type: $name..."
    ssh aetina@$REMOTE_HOST "curl -X DELETE -s $API/$type/$name; curl -X POST -s -H 'Content-Type: application/json' -d @/data/edgex-gui/$file $API/$type"
}

# 1. Update Streams
update_remote "streams" "EdgexStream" "edgex_stream.json"

# 2. Update Rules
update_remote "rules" "face_detection_notification" "face_rule.json"

echo "Remote eKuiper setup complete."
