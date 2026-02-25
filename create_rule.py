import requests
import json

url = "http://localhost:9081/rules"
with open("face_rule.json", "r") as f:
    rule_data = json.load(f)

# Delete rule first to ensure update
requests.delete(url + "/" + rule_data["id"])

response = requests.post(url, json=rule_data)
print(response.status_code)
print(response.text)
