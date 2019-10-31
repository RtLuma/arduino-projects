#!/usr/bin/env python
from sseclient import SSEClient

PROTOCOL="http"
API_URL="192.168.0.69"
SSE_ENDPOINT="events"

messages = SSEClient(f"{PROTOCOL}://{API_URL}/{SSE_ENDPOINT}")
for msg in messages:
    print(msg.data, msg.event, msg.id, msg.retry)