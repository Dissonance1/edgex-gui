#!/bin/bash
# local_setup.sh

API="http://localhost:9081"

# 1. Create Redis Stream
echo "Creating EdgexStream (redisSub)..."
curl -X DELETE -s "$API/streams/EdgexStream"
curl -X POST -s -H 'Content-Type: application/json' -d '{"sql": "CREATE STREAM EdgexStream () WITH (FORMAT=\"JSON\", TYPE=\"redisSub\", DATASOURCE=\"edgex/events/#\")"}' "$API/streams"

# 2. Create Rule
echo "Creating face_detection_notification rule..."
curl -X DELETE -s "$API/rules/face_detection_notification"
# Using the local face_rule.json which uses native EdgexStream
curl -X POST -s -H 'Content-Type: application/json' -d @face_rule.json "$API/rules"

# 3. Create Notification Subscription (REST locally)
echo "Creating Notification Subscription..."
curl -X POST -s -H 'Content-Type: application/json' -d @subscription.json http://localhost:59860/api/v3/subscription

echo "Local eKuiper setup complete."
