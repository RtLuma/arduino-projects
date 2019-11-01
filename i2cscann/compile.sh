#!/usr/bin/bash

# PORT=`arduino-cli board list --timeout 0.5s --format json | jq -r .serialBoards[0].port` # -r raw, omit quotes
PORT="/dev/ttyUSB1"
# PORT=`arduino-cli board list --timeout 0.5s | grep '1a86:7523' | awk '{print $1}'` # -r raw, omit quotes
FQBN="arduino:avr:nano"
SKETCH="${1:-"$PWD"}"
SKETCH_NAME="$(basename $(readlink -e $SKETCH))"
HEX_FILE="$SKETCH/$SKETCH_NAME.${FQBN//:/.}.hex" # double // for global substitute! Bash black magic!
BIN_FILE=${HEX_FILE/\.hex/\.bin}

# arduino-cli compile --show-properties --fqbn $FQBN . ; exit 0

# arduino-cli board attach serial://${PORT}

(
    arduino-cli compile \
    --fqbn $FQBN \
    --build-properties upload.speed=57600 \
    $SKETCH || exit 1
    
    #echo -e "\n=============\n"
    
    #avr-objcopy -I ihex $HEX_FILE -O binary $BIN_FILE 
    
    #"`dirname $0`"/eeprom_metadata.py "`basename $BIN_FILE`"
    
    echo -e "\n=============\n"
)


if [ $? -ne 0 ]; then
    printf "\e[0;31mFAILED\e[0m\n\n"
    exit 1
else
    printf "\e[0;32mBUILT\e[0m\n\n"
fi
