import json
import time
import random
import datetime
import subprocess

def build_v3_event(device_name, confidence):
    ts = int(time.time() * 1000000000)
    event_id = "event-" + str(random.randint(1000, 9999))
    
    event = {
        "apiVersion": "v3",
        "id": event_id,
        "deviceName": device_name,
        "profileName": "face_recog",
        "sourceName": "confidence",
        "origin": ts,
        "readings": [
            {
                "apiVersion": "v3",
                "id": "reading-" + str(random.randint(1000, 9999)),
                "deviceName": device_name,
                "profileName": "face_recog",
                "resourceName": "confidence",
                "origin": ts,
                "valueType": "Float32",
                "value": str(confidence)
            }
        ]
    }
    return event

import base64

import uuid

def build_message_envelope(payload_dict):
    payload_json = json.dumps(payload_dict)
    # EdgeX V3 MessageEnvelope structure (PascalCase for Go-compatible JSON)
    envelope = {
        "ApiVersion": "v3",
        "ContentType": "application/json",
        "Payload": base64.b64encode(payload_json.encode('utf-8')).decode('utf-8'),
        "CorrelationID": str(uuid.uuid4()),
        "RequestID": str(uuid.uuid4())
    }
    return envelope

def simulate():
    device_name = "_Face-recog"
    topic = f"edgex/events/device/face_recog/{device_name}/confidence"
    
    print(f"Starting direct Redis simulation on topic: {topic}")
    
    while True:
        confidence = random.uniform(0.7, 0.95)
        event = build_v3_event(device_name, confidence)
        payload = json.dumps(event)
        
        try:
            print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Publishing (RAW): confidence={confidence:.2f}")
            # Call redis-cli directly since we are already in WSL
            cmd = ["redis-cli", "PUBLISH", topic, payload]
            subprocess.run(cmd, capture_output=True, check=True)
        except Exception as e:
            print(f"  Error: {e}")
            
        time.sleep(random.randint(10, 15))

if __name__ == "__main__":
    simulate()
