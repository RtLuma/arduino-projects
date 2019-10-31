#!/usr/bin/bash

source "`dirname $0`"/compile.sh

if [ $? -ne 0 ]; then exit 0; fi

# arduino-cli upload -v -b $FQBN -p $PORT $SKETCH

PIDS_LISTENING_TO_PORT=`lsof -t $PORT`

for PID in $PIDS_LISTENING_TO_PORT; do
    kill -s SIGTSTP $PID
done

avrdude -C /etc/avrdude.conf -v -patmega328p -carduino -P${PORT} -b57600 -D -Uflash:w:${HEX_FILE}:i 

for PID in $PIDS_LISTENING_TO_PORT; do
    kill -s SIGCONT $PID
done

rm *.elf *.hex *.bin *.eeprom &> /dev/null