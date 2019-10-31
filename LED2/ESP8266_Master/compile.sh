#!/usr/bin/bash
# arduino-cli core update-index
# arduino-cli core upgrade

# PORT=`arduino-cli board list --timeout 0.5s --format json | jq -r .serialBoards[0].port` # -r raw, omit quotes
PORT=${2:-"/dev/ttyUSB0"}
# PORT=`arduino-cli board list --timeout 0.5s | grep '0403:6015' | awk '{print $1}'` # -r raw, omit quotes
FQBN="esp8266:esp8266:thingdev"
SKETCH_PATH="${1:-"$PWD"}"
SKETCH_NAME="$(basename $(readlink -e $SKETCH_PATH))"
SKETCH_SRC="$SKETCH_NAME.ino"
SKETCH_BUILD_PATH="/tmp/${SKETCH_NAME}_build"
SKETCH_CACHE_PATH="/tmp/${SKETCH_NAME}_cache"
BINARY="$SKETCH_BUILD_PATH/$SKETCH_NAME.ino.bin"

# arduino-cli compile --show-properties --fqbn esp8266:esp8266:thingdev  .

# arduino-cli board attach serial://${PORT}

# arduino-cli compile \
# --fqbn $FQBN \
# --build-properties build.f_cpu=160000000L,upload.speed=921600,build.flash_size=512K32 \
# $SKETCH

mkdir -p "$SKETCH_BUILD_PATH" "$SKETCH_CACHE_PATH"

/usr/share/arduino/arduino-builder \
-compile \
-logger=machine \
-hardware /usr/share/arduino/hardware \
-hardware /home/m/.arduino15/packages \
-tools /usr/share/arduino/tools-builder \
-tools /home/m/.arduino15/packages \
-libraries /home/m/Arduino/libraries \
-fqbn=esp8266:esp8266:thingdev:xtal=160,vt=flash,exception=disabled,ssl=all,eesz=512K32,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600 \
-vid-pid=0403_6015 \
-ide-version=10809 \
-build-path "$SKETCH_BUILD_PATH" \
-warnings=none \
-build-cache "$SKETCH_CACHE_PATH" \
-prefs=build.warn_data_percentage=75 \
-prefs=runtime.tools.python.path=/home/m/.arduino15/packages/esp8266/tools/python/3.7.2-post1 \
-prefs=runtime.tools.python-3.7.2-post1.path=/home/m/.arduino15/packages/esp8266/tools/python/3.7.2-post1 \
-prefs=runtime.tools.mkspiffs.path=/home/m/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-3-20ed2b9 \
-prefs=runtime.tools.mkspiffs-2.5.0-3-20ed2b9.path=/home/m/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-3-20ed2b9 \
-prefs=runtime.tools.xtensa-lx106-elf-gcc.path=/home/m/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-3-20ed2b9 \
-prefs=runtime.tools.xtensa-lx106-elf-gcc-2.5.0-3-20ed2b9.path=/home/m/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-3-20ed2b9 \
-verbose "$SKETCH_PATH/$SKETCH_SRC" \

if [ $? -ne 0 ]; then
    printf "\e[0;31mFAILED\e[0m\n"
    exit 1
else
    printf "\e[0;32mBUILT\e[0m\n"
fi