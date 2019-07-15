#!/usr/bin/env bash
TTY_DEVC=`ls -1 /dev/ttyACM* | head -n 1`


/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/bin/avrdude -C/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/etc/avrdude.conf -v -patmega328p -carduino -P${TTY_DEVC} -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0xFD:m -Uhfuse:w:0xDA:m -Ulfuse:w:0xFF:m

/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/bin/avrdude -C/home/m/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14/etc/avrdude.conf -v -patmega328p -carduino -P${TTY_DEVC} -b19200 -Uflash:w:/home/m/.arduino15/packages/arduino/hardware/avr/1.6.23/bootloaders/atmega/ATmegaBOOT_168_atmega328.hex:i -Ulock:w:0x0F:m
