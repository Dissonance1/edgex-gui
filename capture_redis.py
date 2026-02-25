import redis
import json

r = redis.Redis(host='localhost', port=6379, db=0)
p = r.pubsub()
p.psubscribe('edgex.events.#')

print("Waiting for message...")
for message in p.listen():
    if message['type'] == 'pmessage':
        print(f"Topic: {message['channel']}")
        print(f"Data: {message['data'].decode('utf-8', errors='ignore')}")
        break
