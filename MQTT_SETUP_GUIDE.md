# EdgeX MQTT Integration Setup Guide for Eurth LoRa Sensor

## Overview

This guide explains how to configure EdgeX to receive MQTT data directly from your Eurth LoRa sensor via ChirpStack. The data flows as follows:

```
Eurth LoRa Sensor → ChirpStack → MQTT Broker → EdgeX device-mqtt → EdgeX Core → App Service → HTTP Export
```

## Prerequisites

- EdgeX Foundry running (core services)
- ChirpStack LoRa Network Server configured
- MQTT broker at 192.168.42.1:1883 with TLS
- Credentials: admin/admin

## Configuration Files Created

### 1. Device Profile
**Location:** `/home/amit/src/edgex/device-mqtt-go/cmd/res/profiles/eurth.lora.device.profile.yml`

Defines the data structure for the Eurth sensor supporting:
- Temperature (Celsius)
- Humidity (%RH)
- IMU (pitch, roll, yaw in degrees)
- Distance (cm)
- SenML JSON object

### 2. Device Definition
**Location:** `/home/amit/src/edgex/device-mqtt-go/cmd/res/devices/eurth.lora.device.yaml`

Defines the Eurth device instance and MQTT topic subscription pattern: `application/+/device/eurth/event/up`

### 3. Device-MQTT Configuration
**Location:** `/home/amit/src/edgex/device-mqtt-go/cmd/res/configuration.yaml`

Updated with:
- MQTT broker: 192.168.42.1:1883
- TLS enabled
- Authentication: admin/admin
- Topic: `application/+/device/+/event/up`

### 4. ChirpStack Decoder
**Location:** `/home/amit/src/edgex/chirpstack-decoder-senml.js`

JavaScript codec for ChirpStack that converts LoRa payload to SenML format.

## Setup Steps

### Step 1: Configure ChirpStack Decoder

1. Log into your ChirpStack Application Server
2. Navigate to your application → Device Profiles
3. Find or create the device profile for Eurth sensor
4. Go to the "Codec" tab
5. Select "JavaScript" as codec type
6. Copy the contents of `/home/amit/src/edgex/chirpstack-decoder-senml.js` into the decoder field
7. Save the configuration

### Step 2: Configure ChirpStack MQTT Integration

1. In ChirpStack, go to Applications → Your Application → Integrations
2. Add/Edit MQTT integration
3. Ensure it publishes to topic: `application/{applicationID}/device/{devEUI}/event/up`
4. The integration should use your MQTT broker at 192.168.42.1:1883

### Step 3: Build and Run EdgeX device-mqtt Service

```bash
cd /home/amit/src/edgex/device-mqtt-go
make build
./bin/device-mqtt
```

Or if using Docker:
```bash
make docker
# Then update your docker-compose to use the new image
```

### Step 4: Verify Device Registration

Check that the Eurth device is registered in EdgeX:

```bash
curl http://localhost:59881/api/v3/device/name/eurth-lora-sensor | jq
```

### Step 5: Test MQTT Data Flow

Publish a test message to simulate sensor data:

```bash
mosquitto_pub -h 192.168.42.1 -p 1883 -u admin -P admin \
  --cafile /path/to/ca.crt \
  -t "application/1/device/eurth/event/up" \
  -m '{
    "applicationID": "1",
    "deviceName": "eurth",
    "devEUI": "0000000000000001",
    "fPort": 2,
    "data": [
      {"bn": "urn:dev:eurth:", "n": "temperature", "u": "Cel", "v": 22.5, "t": "2025-12-16T09:30:00Z"}
    ]
  }'
```

### Step 6: Verify Data in EdgeX

Check EdgeX Core Data for events:

```bash
# Get all events from eurth device
curl http://localhost:59880/api/v3/event/device/name/eurth-lora-sensor | jq

# Get latest event
curl http://localhost:59880/api/v3/event/device/name/eurth-lora-sensor?limit=1 | jq
```

### Step 7: Configure App Service (Optional)

If you want to transform and export the LoRa data to HTTP endpoints like your face recognition setup:

1. Update `/home/amit/src/edgex/app-functions-sdk-go/app-service-template/res/configuration.yaml`
2. Add "eurth-lora-sensor" to the device filter if needed
3. The existing SenML transformation may need updates to handle LoRa sensor data

## Data Format

### ChirpStack MQTT Message (with decoder)
```json
{
  "applicationID": "1",
  "applicationName": "eurth-app",
  "deviceName": "eurth",
  "devEUI": "0000000000000001",
  "fPort": 2,
  "data": [
    {
      "bn": "urn:dev:eurth:",
      "n": "temperature",
      "u": "Cel",
      "v": 22.5,
      "t": "2025-12-16T09:30:00Z"
    }
  ]
}
```

### EdgeX Event (after device-mqtt ingestion)
```json
{
  "apiVersion": "v3",
  "id": "...",
  "deviceName": "eurth-lora-sensor",
  "profileName": "Eurth-LoRa-Sensor-Profile",
  "sourceName": "senml_data",
  "origin": 1702729800000000000,
  "readings": [
    {
      "id": "...",
      "origin": 1702729800000000000,
      "deviceName": "eurth-lora-sensor",
      "resourceName": "senml_data",
      "profileName": "Eurth-LoRa-Sensor-Profile",
      "valueType": "Object",
      "objectValue": {
        "bn": "urn:dev:eurth:",
        "n": "temperature",
        "u": "Cel",
        "v": 22.5,
        "t": "2025-12-16T09:30:00Z"
      }
    }
  ]
}
```

## Troubleshooting

### Issue: device-mqtt can't connect to broker

**Check:**
- Broker is accessible: `telnet 192.168.42.1 1883`
- Credentials are correct
- TLS certificates if required (update configuration.yaml with cert paths)

**Solution:**
If TLS certificate verification fails, you can temporarily set `SkipCertVerify: true` in configuration.yaml (not recommended for production).

### Issue: No events appearing in EdgeX

**Check:**
1. device-mqtt logs: `journalctl -u device-mqtt -f` or check console output
2. MQTT broker logs
3. Verify topic subscription matches publish topic
4. Test with mosquitto_sub:
   ```bash
   mosquitto_sub -h 192.168.42.1 -p 1883 -u admin -P admin -t "application/+/device/+/event/up" -v
   ```

### Issue: Data format errors

**Check:**
- ChirpStack decoder is properly configured
- Decoder outputs valid SenML JSON
- Test decoder in ChirpStack UI with sample payload

### Issue: TLS connection fails

**Solutions:**
1. Verify TLS is actually required (try changing schema to "tcp" temporarily)
2. If using self-signed certificates, add CA certificate path to configuration.yaml:
   ```yaml
   CaCertFile: "/path/to/ca-cert.pem"
   ```
3. For testing only, enable `SkipCertVerify: true`

## Next Steps

1. **Monitor the data flow** - Watch logs from device-mqtt service
2. **Integrate with app service** - If you want to export to HTTP endpoints
3. **Add more sensors** - Create additional device definitions for other LoRa sensors
4. **Secure the setup** - Use proper TLS certificates and secure credential storage

## Sensor Types by fPort

The Eurth sensor supports multiple sensor types identified by fPort:

| fPort | Sensor Type | Readings |
|-------|-------------|----------|
| 2 | Temperature | temperature (Celsius) |
| 6 | Humidity | humidity (%RH) |
| 3 | IMU | pitch, roll, yaw (degrees) |
| 4 | Distance | distance (cm) |

Each sensor type will generate separate MQTT messages with the corresponding SenML data.
