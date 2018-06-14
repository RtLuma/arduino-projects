#include <Wire.h>
#include <EEPROM.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"

#define EEPROM_MODE_ADDR 0
#define EEPROM_RGB_ADDR 1

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

volatile uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
volatile uint8_t rgb[3] = {255, 32, 0};

const byte hues[BANDS] = { 0, 20, 45, 105, 140, 185, 215 };

typedef void (*SimplePatternList[])();

SimplePatternList gPatterns = {
  []() { theaterChase(rgb[0], rgb[1], rgb[2], 1); },
  []() { showColor(rgb[0], rgb[1], rgb[2]); delay(1);},
  []() { colorWipe(rgb[0], rgb[1], rgb[2], 1); },
  []() { rainbowCycle(5 , 3); },
  []() {

    FFT.read();

    byte magnitudes[BANDS][2][3];
    byte t = 0;
    //    byte t = millis() >> 8;
    for (byte band = 0; band < BANDS; band++) {

      byte hue = hues[band];

      uint32_t ratios[3] = { rainbow(hue + 170), rainbow(hue + 85), rainbow(hue) };

      for (byte channel = 0; channel < 2; channel++) {
        uint32_t* level = channel ? FFT.R : FFT.L;
        uint32_t* thresh = channel ? FFT.Rmax : FFT.Lmax;

        uint32_t numer = uint32_t(level[band] >> 6);
        uint32_t denom = uint32_t(thresh[band] >> 6);

        for (byte i = 0; i < 3; i++) {
          ratios[0] *= numer;
          ratios[0] /= denom;
          ratios[1] *= numer;
          ratios[1] /= denom;
          ratios[2] *= numer;
          ratios[2] /= denom;
        }

        magnitudes[band][channel][0] = ratios[0];
        magnitudes[band][channel][1] = ratios[1];
        magnitudes[band][channel][2] = ratios[2];
      }
    }

    for (byte band = 0; band < BANDS; band++) {
      for (byte p = 0; p < 43; p++ ) {
        sendPixel(magnitudes[band][0][0], magnitudes[band][0][1], magnitudes[band][0][2]);
      }
    }
    for (char band = BANDS - 1; band > -1; band--) {
      for (byte p = 0; p < 43; p++ ) {
        sendPixel(magnitudes[band][1][0], magnitudes[band][1][1], magnitudes[band][1][2]);
      }
    }
    show();
    //    delay(10);
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
    rgb[i] = Wire.read(); // receive byte as a character
    i++;
  }

  for (i = 0; i < 3; i++) EEPROM.write(EEPROM_RGB_ADDR + i, rgb[i]);

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
  for (byte i = 0; i < 3; i++) rgb[i] = EEPROM.read(EEPROM_RGB_ADDR + i);
}

void loop() {
  //  showColor(255, 64, 0);
  gPatterns[gCurrentPatternNumber]();

  // Some example procedures showing how to display to the pixels:

  //  showColor(125, 125, 125);
  //  delay(5000);
  //
  //  colorWipe(255, 32, 0, 5); // Red
  //  colorWipe(0, 255, 0, 5); // Green
  //  colorWipe(0, 0, 255, 5); // Blue
  //  //    colorWipe(200, 200, 200, 500);
  //
  //  // Send a theater pixel chase in...
  //  theaterChase(127, 127, 127, 20); // White
  //  //    show();
  //  //    theaterChase(127,   0,   0, 5); // Red
  //  theaterChase(  0,   64, 127, 20); // Blue
  //
  //  rainbowCycle(1000 , 20 , 5, 20);
  //  //  for (int i = 0; i < PIXELS; i++)  rainbowCycle(1000 , 20 , i );
  //  //    detonate( 200 , 200 , 200 , 1000);
  //  //delay(100);

}






