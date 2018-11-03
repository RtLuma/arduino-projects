#define DATA_PIN 7
#define PIXELS 150
#define EEPROM_MODE_ADDR 0
#define EEPROM_RGB_ADDR 1
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include <Wire.h>
#include <EEPROM.h>
#include "WS2812.h"
#include "Util.h"
#include "Spectrum.h"
#include "Modes.h"


volatile byte mode = 0; // Index number of which pattern is current
volatile byte RGB[3] = {255, 32, 0};

WS2812 LED(PIXELS);
cRGB p;

void (*modes[])() =  {
  rainbowWave,
  rainbowFill,
  []() { fillColor(RGB[0], RGB[1], RGB[2]); delay(5000); },
};

void nextPattern() {
  mode = (mode + 1) % ARRAY_SIZE( modes);
  EEPROM.write(EEPROM_MODE_ADDR, mode);
  
  ((void (*)())NULL)(); //Software reset
}

void receiveEvent(int howMany) {  
  cli();
  if (howMany < 3) nextPattern();

  byte i = 0;

  while (Wire.available()) { // loop through all but the last
    RGB[i] = Wire.read(); // receive byte as a character
    i++;
  }

  for (i = 0; i < 3; i++) EEPROM.write(EEPROM_RGB_ADDR + i, RGB[i]);
  if (Serial.available()) Serial.println("dick");
  Wire.flush();
  sei();
}

void (*show)(void);

void setup() {
//  Serial.begin(115200);
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  LED.setOutput(DATA_PIN);
  LED.setColorOrderGRB();
  mode = EEPROM.read(EEPROM_MODE_ADDR);
  show = modes[mode];
}

void loop() {
    show();
//  rainbowWave();
  //  FFT.read();
  //  for (byte band = 0; band < 7; band++)
  //  fillColor(p);
}


