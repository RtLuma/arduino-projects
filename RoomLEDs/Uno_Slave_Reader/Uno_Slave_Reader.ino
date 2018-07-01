volatile uint8_t R = 255, G = 32, B = 0, F = 0, P = 0, W = 0;
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include <Wire.h>
#include <EEPROM.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"
#include "Visualizer.h"
#include "LabelMaps.h"

void receiveEvent(byte length) {
  cli();
  uint8_t i = 0;
  uint8_t arg_num = length >> 1;
  char    labels[arg_num];
  uint8_t values[arg_num];

  while (Wire.available()) {
    labels[i] = Wire.read(); // receive byte as a character
    values[i] = Wire.read();
    i++;
  }
  Wire.flush();
  bool needSoftReset = false;

  for (i = 0; i < arg_num; i++) {
    switch (labels[i]) {
      case 'M': {
          int8_t modeIndex = label2modeIndex(values[i]);
          if (modeIndex > -1) {
            EEPROM.write(e_mode, modeIndex);
            needSoftReset = true;
            mode = modes[modeIndex];
          }
        }
      case 'R': R = values[i]; EEPROM.write(e_red,   R); 
      case 'G': G = values[i]; EEPROM.write(e_green, G); 
      case 'B': B = values[i]; EEPROM.write(e_blue,  B); 
      case 'F': F = values[i]; EEPROM.write(e_freq,  F); 
      case 'P': P = values[i]; EEPROM.write(e_per,   P); 
      case 'W': W = values[i]; EEPROM.write(e_width, W); 
    }
  }
  if (needSoftReset) ((void (*)())NULL)();
  else if (!EEPROM.read(e_mode)) mode();
  sei();
}


void setup() {
  Serial.begin(9600);

  // Load user settings
  R = EEPROM.read(e_red);
  G = EEPROM.read(e_green);
  B = EEPROM.read(e_blue);
  F = EEPROM.read(e_freq);
  P = EEPROM.read(e_per);
  W = EEPROM.read(e_width);

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  FFT.begin();  // Init Audio-Spectrum shield
  //      mode = modes[0];
  mode = modes[EEPROM.read(e_mode)]; // Load display mode

  ledsetup();
//  showColor(0, 0, 0); //blank the display
}

void loop() {
  mode();
}





