#!/usr/bin/bash

PORT=${1:-`arduino-cli board list --timeout 0.1s | grep '0403:6015' | awk '{print $1}'`}
BAUD=${2:-`stty -F ${PORT} | grep baud | awk '{print $2}'`}
# BAUD=115200

python3 monitor.py $PORT $BAUD
