#!/bin/bash

MQTT_HOST="localhost"
MQTT_PORT=1883
MQTT_TOPIC="Honeycomb/device/2cf7f12052608e69"

# Generate random float in range
rand_float () {
    local min=$1
    local max=$2
    awk -v min="$min" -v max="$max" \
        'BEGIN { srand(); printf "%.6f", min + rand() * (max - min) }'
}

publish_data() {
    local count=$1

    for i in $(seq 1 "$count"); do

        # Random IMU values
        PITCH=$(rand_float -10 10)     # degrees
        ROLL=$(rand_float -10 10)      # degrees
        YAW=$(rand_float 0 360)        # degrees

        # Current UTC timestamp (EdgeX compatible)
        TIMESTAMP=$(date -u "+%Y-%m-%d %H:%M:%S UTC")

        PAYLOAD=$(cat <<EOF
[
  {
    "bn": "2cf7f12052608e69",
    "n": "_pitch_x",
    "timestamp": "$TIMESTAMP",
    "u": "deg",
    "v": $PITCH
  },
  {
    "bn": "2cf7f12052608e69",
    "n": "_roll_y",
    "timestamp": "$TIMESTAMP",
    "u": "deg",
    "v": $ROLL
  },
  {
    "bn": "imu_2cf7f12052608e69",
    "n": "_yaw_z",
    "timestamp": "$TIMESTAMP",
    "u": "deg",
    "v": $YAW
  }
]
EOF
)

        echo "[$i] Publishing randomized IMU payload to $MQTT_TOPIC:"
        echo "$PAYLOAD"

        echo "$PAYLOAD" | mosquitto_pub \
            -h "$MQTT_HOST" \
            -p "$MQTT_PORT" \
            -t "$MQTT_TOPIC" \
            -s

        sleep 2
    done
}

# Default publish count = 5
if [ -z "$1" ]; then
    publish_data 5
else
    publish_data "$1"
fi

echo "Done."
