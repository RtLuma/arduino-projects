#!/usr/bin/bash

source "`dirname $0`"/compile.sh

if [ $? -ne 0 ]; then exit 0; fi


# PIDS_LISTENING_TO_PORT=`lsof -t $PORT`

# for PID in $PIDS_LISTENING_TO_PORT; do
#     kill -s SIGTSTP $PID
#     # kill $PID
# done



/home/m/.arduino15/packages/esp8266/tools/python/3.7.2-post1/python \
/home/m/.arduino15/packages/esp8266/hardware/esp8266/2.5.2/tools/upload.py \
--chip esp8266 \
--port /${PORT} \
--baud 921600 \
--trace version \
--end \
--chip esp8266 \
--port /${PORT} \
--baud 921600 \
--trace write_flash 0x0 "$BINARY" --end 



# for PID in $PIDS_LISTENING_TO_PORT; do
#     kill -s SIGCONT $PID
# done

# rm *.elf *.hex *.bin &> /dev/null