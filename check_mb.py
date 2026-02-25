import requests
import json

try:
    r = requests.get('http://localhost:59880/api/v3/config')
    if r.status_code == 200:
        conf = r.json().get('config', {})
        print("MessageBus:", json.dumps(conf.get('MessageBus', {}), indent=2))
        print("MessageQueue:", json.dumps(conf.get('MessageQueue', {}), indent=2))
    else:
        print(f"Error: {r.status_code}")
except Exception as e:
    print(f"Failed: {e}")
