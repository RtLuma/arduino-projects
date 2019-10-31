#!/usr/bin/env python
from sseclient import SSEClient, requests

PROTOCOL="http"
API_URL="192.168.0.69"
SSE_ENDPOINT="events"
TRIGGER_ENDPOINT="longreply"

messages = SSEClient(f"{PROTOCOL}://{API_URL}/{SSE_ENDPOINT}")
print(requests.get(f"{PROTOCOL}://{API_URL}/{TRIGGER_ENDPOINT}").status_code)
for msg in messages:
    print(msg.data, msg.event, msg.id, msg.retry)