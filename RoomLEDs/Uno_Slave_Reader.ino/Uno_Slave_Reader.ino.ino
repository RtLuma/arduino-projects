#include <Wire.h>
#include <EEPROM.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"

#define EEPROM_MODE_ADDR 0
#define EEPROM_RGB_ADDR 1

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define SECTION 23

volatile uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
volatile uint8_t RGB[3] = {255, 32, 0};
uint8_t colors[2][BANDS + 1][3] = {0};
int32_t spiral = 0;

//const byte hues[BANDS] = { 2, 21, 45, 115, 150, 200, 225 };   // Pretty
const byte hues[BANDS] = { 0, 37, 74, 111, 148, 185, 222 };   // Even

typedef void (*SimplePatternList[])();

uint32_t counter = 0;

SimplePatternList gPatterns = {
  []() { theaterChase(RGB[0], RGB[1], RGB[2], 1); },
  []() { showColor(RGB[0], RGB[1], RGB[2]); delay(1);},
  //  []() { colorWipe(RGB[0], RGB[1], RGB[2], 1); },
  []() { rainbowCycle(5 , 3); },
  []() {

    FFT.read();

    byte t = millis() >> 8;

    for (uint8_t channel = 0; channel < 2; channel++) {
      for (uint8_t band = 0; band < BANDS; band++) {

        uint8_t hue = hues[band] - t;

        uint32_t level = FFT.lvls[channel][band] + 1;

        colors[channel][band][0] = (rainbow(hue + 170)  * level) >> 10;
        colors[channel][band][1] = (rainbow(hue + 85)   * level) >> 11;
        colors[channel][band][2] = (rainbow(hue)        * level) >> 10;
      }

      //Prevent segfault
      colors[channel][BANDS][0] = colors[channel][BANDS - 1][0];
      colors[channel][BANDS][1] = colors[channel][BANDS - 1][1];
      colors[channel][BANDS][2] = colors[channel][BANDS - 1][2];
    }

    /////////////////////////////// start computing and broadcasting each pixel on the fly

    int8_t nextband;
    uint8_t dist, band;
    uint8_t pixelAr, pixelAg, pixelAb, pixelBr, pixelBg, pixelBb;
    uint8_t *pixelA, *pixelB;


    for (band = 0; band < BANDS; band++) {

      dist = SECTION;

      pixelA = colors[0][band];
      pixelB = colors[0][band + 1];

      pixelAr = pixelA[0];
      pixelAg = pixelA[1];
      pixelAb = pixelA[2];

      pixelBr = pixelB[0];
      pixelBg = pixelB[1];
      pixelBb = pixelB[2];

      for (uint8_t p = 0; p < SECTION; p++) {
        dist--;
        sendPixel(
          ((pixelAr * dist) + (pixelBr * p)) / SECTION,
          ((pixelAg * dist) + (pixelBg * p)) / SECTION,
          ((pixelAb * dist) + (pixelBb * p)) / SECTION
        );
      }
    }

    for (band = BANDS - 1; band > 0; band--) {
      dist = SECTION;

      pixelA = colors[1][band];
      pixelB = colors[1][band - 1];

      pixelAr = pixelA[0];
      pixelAg = pixelA[1];
      pixelAb = pixelA[2];

      pixelBr = pixelB[0];
      pixelBg = pixelB[1];
      pixelBb = pixelB[2];

      for (uint8_t p = 0; p < SECTION; p++) {
        dist--;
        sendPixel(
          ((pixelAr * dist) + (pixelBr * p)) / SECTION,
          ((pixelAg * dist) + (pixelBg * p)) / SECTION,
          ((pixelAb * dist) + (pixelBb * p)) / SECTION
        );
      }
    }
    sendPixel(colors[1][0][0], colors[1][0][1], colors[1][0][2]);

    uint32_t sum_vals = 0, sum_maxs = 0;

    for (uint8_t band = 0; band < BANDS; band++) {
      sum_vals += (FFT.lvls[0][band] + FFT.lvls[1][band]) >> 1;
      sum_maxs += (FFT.maxs[0][band] + FFT.maxs[1][band]) >> 1;
    }

    spiral += (sum_vals * 300) / sum_maxs;
    spiral >>= 1;
    //    spiral = (sum_vals * spiral) / sum_maxs;

    spiral = min(spiral, 300);

    byte magn = (spiral * 255) / 300;
        magn = (spiral * magn) / 300;
    magn >>= 1;


    int16_t p = 0;
    //for (p; p < spiral; p++) sendPixel2(magn, magn, magn);
    //for (p; p < 300; p++) sendPixel2(0, 0, 0);

    for (p; p < 300 - spiral; p++) sendPixel2(0, 0, 0);
    for (p; p < 300; p++) sendPixel2(magn, magn, magn);

  }
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
    //    resetFunc();
    //    ((void(*)(void))&loop)();
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






