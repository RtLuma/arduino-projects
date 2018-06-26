#include <Wire.h>
#include <EEPROM.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"
#include "Visualizer.h"

#define EEPROM_MODE_ADDR 0
#define EEPROM_RGB_ADDR 1

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//const byte hues[BANDS] = { 2, 21, 45, 115, 150, 200, 225 };   // Pretty
//const byte hues[BANDS] = { 2, 21, 35, 185, 200, 220, 240 };   // Sunset
const uint8_t hues[BANDS] = { 0, 37, 74, 111, 148, 185, 222 };   // Even

volatile uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
volatile uint8_t RGB[3] = {255, 32, 0};



typedef void (*SimplePatternList[])();

uint32_t counter = 0;

SimplePatternList gPatterns = {
  []() { theaterChase(RGB[0], RGB[1], RGB[2]); },
  []() { showColor(RGB[0], RGB[1], RGB[2]); delay(1);},
  //  []() { colorWipe(RGB[0], RGB[1], RGB[2], 1); },
//  []() { rainbowCycle(5 , 3); },
  Chroma
};

void nextPattern() {
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  EEPROM.write(EEPROM_MODE_ADDR, gCurrentPatternNumber);
}


void receiveEvent(int howMany) {
  cli();

  if (howMany < 3) {
    nextPattern();
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
  FFT.begin();

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  gCurrentPatternNumber = EEPROM.read(EEPROM_MODE_ADDR);
  for (byte i = 0; i < 3; i++) RGB[i] = EEPROM.read(EEPROM_RGB_ADDR + i);

  showColor(0, 0, 0);
}

void loop() {
  gPatterns[gCurrentPatternNumber]();
}






