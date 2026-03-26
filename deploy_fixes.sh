#!/bin/bash

# EdgeX Notification Fix Deployment Script
# This script applies the corrected Stream, Rule, and Subscription configurations.
# It assumes the JSON files (edgex_stream.json, face_rule.json, subscription.json) are in the same directory.
# Run this script on the device or via SSH: ssh user@device 'bash -s' < deploy_fixes.sh

KUIPER_API="http://localhost:9081"
NOTIFY_API="http://localhost:59860/api/v3"

echo "Started applying EdgeX Notification fixes..."

# 1. Recreate Native EdgeX Stream
echo "[1/3] creating native EdgexStream..."
curl -X DELETE -s "$KUIPER_API/streams/EdgexStream" > /dev/null
curl -X POST -s -H 'Content-Type: application/json' -d '{"sql": "CREATE STREAM EdgexStream () WITH (FORMAT=\"JSON\", TYPE=\"edgex\")"}' "$KUIPER_API/streams"
if [ $? -eq 0 ]; then echo "  ✓ EdgexStream created"; else echo "  ✗ Failed to create stream"; fi

# 2. Recreate Notification Rule
echo "[2/3] Deploying Face Detection Notification Rule..."
curl -X DELETE -s "$KUIPER_API/rules/face_detection_notification" > /dev/null
# Note: Ensure face_rule.json is available on the device or embed content here
curl -X POST -s -H 'Content-Type: application/json' -d @face_rule.json "$KUIPER_API/rules"
if [ $? -eq 0 ]; then echo "  ✓ Rule deployed"; else echo "  ✗ Failed to deploy rule"; fi

# 3. Create Notification Subscription
echo "[3/3] Creating Notification Subscription..."
# Note: Ensure subscription.json is available on the device or embed content here
curl -X POST -s -H 'Content-Type: application/json' -d @subscription.json "$NOTIFY_API/subscription"
if [ $? -eq 0 ]; then echo "  ✓ Subscription created"; else echo "  ✗ Failed to create subscription"; fi

echo "Deployment complete. Monitor logs with: docker logs edgex-support-notifications"
