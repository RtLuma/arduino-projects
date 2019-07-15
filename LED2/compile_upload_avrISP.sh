#!/usr/bin/env bash
SOURCE="$1"
[[ "$SOURCE" =~ (.+)(\..+) ]] && EXTENTIONLESS="${BASH_REMATCH[1]}" EXTENTION="${BASH_REMATCH[2]}"
TTY_DEVICE=`ls -1 /dev/ttyACM* | head -n 1`

if [[ "$EXTENTION" == ".ino" ]]; then   # Have to build proper .cpp source from ino
    /opt/Arduino/arduino-builder -dump\
	-prefs -logger=machine -hardware /opt/Arduino/hardware -hardware ~/.arduino15/packages -tools /opt/Arduino/tools-builder -tools /opt/Arduino/hardware/tools/avr -tools ~/.arduino15/packages -built-in-libraries /opt/Arduino/libraries -libraries ~/Arduino/libraries -fqbn=arduino:avr:nano:cpu=atmega328old -vid-pid=0X2341_0X0043 -ide-version=10809 -build-path .\
	-prefs=build.warn_data_percentage=75 \
	-prefs=runtime.tools.avrdude.path=~/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14 \
	-prefs=runtime.tools.avrdude-6.3.0-arduino14.path=~/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14 \
	-prefs=runtime.tools.avr-gcc.path=~/.arduino15/packages/arduino/tools/avr-gcc/5.4.0-atmel3.6.1-arduino2 \
	-prefs=runtime.tools.avr-gcc-5.4.0-atmel3.6.1-arduino2.path=~/.arduino15/packages/arduino/tools/avr-gcc/5.4.0-atmel3.6.1-arduino2 

    /opt/Arduino/arduino-builder -compile -logger=machine -hardware /opt/Arduino/hardware -hardware ~/.arduino15/packages -tools /opt/Arduino/tools-builder -tools /opt/Arduino/hardware/tools/avr -tools ~/.arduino15/packages -built-in-libraries /opt/Arduino/libraries -libraries ~/Arduino/libraries -fqbn=arduino:avr:nano:cpu=atmega328old -vid-pid=0X2341_0X0043 -ide-version=10809 -build-path . \
	-prefs=build.warn_data_percentage=75 \
	-prefs=runtime.tools.avrdude.path=~/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14 \
	-prefs=runtime.tools.avrdude-6.3.0-arduino14.path=~/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino14 \
	-prefs=runtime.tools.avr-gcc.path=~/.arduino15/packages/arduino/tools/avr-gcc/5.4.0-atmel3.6.1-arduino2 \
	-prefs=runtime.tools.avr-gcc-5.4.0-atmel3.6.1-arduino2.path=~/.arduino15/packages/arduino/tools/avr-gcc/5.4.0-atmel3.6.1-arduino2 

fi

#Generate hex
avr-g++ -c -g -Os -w -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10809 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR -I~/.arduino15/packages/arduino/hardware/avr/1.6.23/cores/arduino -I~/.arduino15/packages/arduino/hardware/avr/1.6.23/variants/eightanaloginputs ${SOURCE} -o ${EXTENTIONLESS}.o

exit
avr-g++ -Os -DF_CPU=16000000L -mmcu=atmega328p -c $SOURCE
avr-g++ -DF_CPU=16000000L -mmcu=atmega328p -o ${EXTENTIONLESS}.elf ${EXTENTIONLESS}.o
avr-objcopy -O ihex ${EXTENTIONLESS}.elf ${EXTENTIONLESS}.hex
rm ${EXTENTIONLESS}.o
rm ${EXTENTIONLESS}.elf

#flash:
# avrdude -v -patmega328p -carduino -P${TTY_DEVICE} -b19200 -D -U flash:w:${EXTENTIONLESS}.hex:i