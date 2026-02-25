import requests
import json

try:
    r = requests.get('http://localhost:59880/api/v3/config')
    if r.status_code == 200:
        data = r.json()
        print("Keys:", data.keys())
        if 'config' in data:
            print("Config Keys:", data['config'].keys())
        elif 'configuration' in data:
            print("Configuration Keys:", data['configuration'].keys())
    else:
        print(f"Error: {r.status_code} - {r.text}")
except Exception as e:
    print(f"Failed: {e}")
