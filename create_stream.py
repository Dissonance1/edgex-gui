import requests
import json

url = "http://localhost:9081/streams"
with open("face_stream.json", "r") as f:
    data = json.load(f)

response = requests.post(url, json=data)
print(response.status_code)
print(response.text)
