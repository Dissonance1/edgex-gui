import requests
import json
import time
import random
import datetime

def build_senml_payload_from_yaml(device_name, detection_info):
    iso_timestamp = datetime.datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S UTC")
    base_name = device_name
    senml_payload = []

    senml_payload.append({
        "bn": base_name,
        "timestamp": iso_timestamp,
        "n": "DeviceName",
        "u": "name",
        "vs": base_name
    })

    if "face_count" in detection_info:
        senml_payload.append({
            "bn": base_name,
            "timestamp": iso_timestamp,
            "n": "face_count",
            "v": int(detection_info["face_count"])
        })

    if "confidence" in detection_info:
        senml_payload.append({
            "bn": base_name,
            "timestamp": iso_timestamp,
            "n": "confidence",
            "v": float(detection_info["confidence"])
        })

    if "person_name" in detection_info:
        senml_payload.append({
            "bn": base_name,
            "n": "recognized_names",
            "vs": detection_info["person_name"]
        })

    # Add ProfileName for DataIngestionServer translation
    senml_payload.append({
        "n": "ProfileName",
        "vs": "face_recog"
    })

    return senml_payload

def simulate():
    # Target the specific EdgeX V3 device endpoint through the proxy
    url = "http://localhost:4000/api/v3/event/face_service/face_recog/_Face-recog/confidence"
    device_name = "_Face-recog"
    
    print(f"Starting simulation. Sending to {url} every 10-15s...")
    
    while True:
        confidence = random.uniform(0.4, 0.95)
        face_count = random.randint(0, 3)
        person_name = "Person_" + str(random.randint(100, 999)) if face_count > 0 else "None"
        
        detection_info = {
            "face_count": face_count,
            "confidence": confidence,
            "person_name": person_name
        }
        
        payload = build_senml_payload_from_yaml(device_name, detection_info)
        
        try:
            print(f"[{datetime.datetime.now().strftime('%H:%M:%S')}] Sending: face_count={face_count}, confidence={confidence:.2f}")
            response = requests.post(url, json=payload, timeout=5)
            print(f"  Response: {response.status_code}")
            if response.status_code >= 400:
                print(f"  Detail: {response.text}")
        except Exception as e:
            print(f"  Error: {e}")
            
        time.sleep(random.randint(10, 15))

if __name__ == "__main__":
    simulate()
