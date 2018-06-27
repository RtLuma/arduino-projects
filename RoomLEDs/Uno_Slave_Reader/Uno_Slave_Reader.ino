#include <Wire.h>
#include <EEPROM.h>
#include <HashMap.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"
#include "Visualizer.h"

#define EEPROM_MODE_ADDR 0
#define EEPROM_RGB_ADDR 1

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

typedef void (*modePointer)(void);

//const byte hues[BANDS] = { 2, 21, 45, 115, 150, 200, 225 };   // Pretty
//const byte hues[BANDS] = { 2, 21, 35, 185, 200, 22a0, 240 };   // Sunset
const uint8_t hues[BANDS] = { 0, 37, 74, 111, 148, 185, 222 };   // Even

char modeLabel = 'm';
volatile uint8_t RGB[3] = {255, 32, 0};

modePointer mode;
modePointer modes[] = {
  []() { theaterChase(RGB[0], RGB[1], RGB[2]); },
  []() { showColor(RGB[0], RGB[1], RGB[2]); delay(1);},
  //  []() { colorWipe(RGB[0], RGB[1], RGB[2], 1); },
  //  []() { rainbowCycle(5 , 3); },
  Chroma
};
char modeLabels[] = {
  ,'t'//heaterChase
  ,'f'//ill color
  ,'m'//usic visualizer
};

const PROGMEM byte HASH_SIZE = ARRAY_SIZE(modes);
HashType<char, modePointer> hashRawArray[HASH_SIZE] = {};
HashMap<char, modePointer> modeMap = HashMap<char, modePointer>(hashRawArray, ARRAY_SIZE(hashRawArray));

void modeChange(char m) {
  mode = m;
  EEPROM.write(EEPROM_MODE_ADDR, mode);
}


void receiveEvent(int howMany) {
  cli();

  //NO
  //BAD
  //BAD MICHAEL
  //NO

  //New I2C protocol:
  // [ 0, '\r', char<mode>, ... ]
  //NOT < 3
  
  if (howMany < 3) {
    modeChange();
    ((void (*)())NULL)();
    return;
  }

  byte i = 0;

  while (Wire.available()) { // loop through all but the last
    RGB[i] = Wire.read(); // receive byte as a character
    i++;
  }

  for (i = 0; i < 3; i++) EEPROM.write(EEPROM_RGB_ADDR + i, RGB[i]);

  Wire.flush();

  sei();
}


void setup() {
  Serial.begin(115200);

  ledsetup();
  showColor(0, 0, 0);
  
  FFT.begin();

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  for (byte i = 0; i < 3; i++) RGB[i] = EEPROM.read(EEPROM_RGB_ADDR + i);
  for (byte m=0; m < HASH_SIZE; m++) modeMap[m](modeLabels[m], modes[m]);
  showColor(0, 0, 0);
  
  mode = modeMap.getValueOf(EEPROM.read(EEPROM_MODE_ADDR));
}

void loop() {
  mode();
}






