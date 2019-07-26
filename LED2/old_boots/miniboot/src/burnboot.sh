#!/usr/bin/env bash
#avrdude -P /dev/ttyACM0 -b 19200 -c avrisp -p m328p -v -e -U efuse:w:0x05:m -U hfuse:w:0xDE:m -U lfuse:w:0xFF:m
#avrdude -P /dev/ttyACM0 -b 19200 -c avrisp -p m328p -v -e -F -U flash:w:miniboot.hex -U lock:w:0x0F:m
#avrdude -P /dev/ttyACM0 -b 19200 -c avrisp -p m328p -v -e -F -U flash:w:miniboot.hex

BOOT_HEX="miniboot.hex"
TTY_DEV=`ls -1 /dev/ttyACM* | head -n 1`

echo "Using programmer on $TTY_DEV"

/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/bin/avrdude \
-C/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/etc/avrdude.conf \
-v \
-patmega328p \
-carduino \
-P${TTY_DEV} \
-b19200 \
-e \
-Ulock:w:0x3F:m \
-Uefuse:w:0xFD:m \
-Uhfuse:w:0xDA:m \
-Ulfuse:w:0xFF:m 

/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/bin/avrdude \
-C/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/etc/avrdude.conf \
-v \
-patmega328p \
-carduino \
-P${TTY_DEV} \
-b19200 \
-Uflash:w:${BOOT_HEX}:i \
-Ulock:w:0x0F:m
