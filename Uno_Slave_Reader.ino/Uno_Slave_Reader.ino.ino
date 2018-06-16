#include <Wire.h>
#include <EEPROM.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"

#define EEPROM_MODE_ADDR 0
#define EEPROM_RGB_ADDR 1

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
//#define SECTION (PIXELS / (BANDS * 2))
#define SECTION 22
#define MAKE_HUE(x) (x * 255) / BANDS

const byte HUES[BANDS] = { MAKE_HUE(0), MAKE_HUE(1), MAKE_HUE(2), MAKE_HUE(3), MAKE_HUE(4), MAKE_HUE(5), MAKE_HUE(6) };

volatile uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
volatile uint8_t RGB[3] = {255, 32, 0};

//const byte hues[BANDS] = { 2, 21, 45, 115, 150, 200, 225 };

typedef void (*SimplePatternList[])();

SimplePatternList gPatterns = {
  []() { theaterChase(RGB[0], RGB[1], RGB[2], 1); },
  []() { showColor(RGB[0], RGB[1], RGB[2]); delay(1);},
  []() { colorWipe(RGB[0], RGB[1], RGB[2], 1); },
  []() { rainbowCycle(5 , 3); },
  []() {

    FFT.read();

        byte t = millis() >> 6;

    uint16_t colors[2][BANDS + 1][3];

    for (byte channel = 0; channel < 2; channel++) {
      for (byte band = 0; band < BANDS; band++) {
//        byte hue = HUES[band];
        byte hue = 0;
        byte rgb[3] = { rainbow(hue + 170), rainbow(hue + 85), rainbow(hue) };
        for (byte color = 0; color < 3; color++)
          colors[channel][band][color] = (rgb[color] * (FFT.lvls[channel][band] + 1)) >> 10;
      }
      for (byte color = 0; color < 3; color++)
        colors[channel][BANDS][color] = colors[channel][0][color];  //Prevent segfault, fade back to start color
    }

    for (byte channel = 0; channel < 2; channel++) {
//      byte channel = 0;
      for (byte band = 0; band < BANDS; band++) {
        for (byte pixel = 0; pixel < SECTION; pixel++) {
//          auto pixelA = colors[channel][band];
//          auto pixelB = colors[channel][band + 1];
//          uint16_t dist = SECTION - pixel;
          sendPixel(
//            ((pixelA[0]*dist) + (pixelB[0]*pixel)) / SECTION,
//            ((pixelA[1]*dist) + (pixelB[1]*pixel)) / SECTION,
//            ((pixelA[2]*dist) + (pixelB[2]*pixel)) / SECTION
              colors[channel][band][0],colors[channel][band][1],colors[channel][band][2]
          );
        }
      }
    }
    show();
    //    delayMicroseconds(120);
  },


  []() {
    byte t = millis() >> 3;
    for (uint16_t pixel = 0; pixel < PIXELS; pixel++) {
      byte hue = map(PIXELS - pixel, 0, PIXELS, 0, 255) + t;
      sendPixel(rainbow(hue + 170), rainbow(hue + 85), rainbow(hue));
      //      delayMicroseconds(70);
    }
    show();
    delayMicroseconds(120);
  },


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






