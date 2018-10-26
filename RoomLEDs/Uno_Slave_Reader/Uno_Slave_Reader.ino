volatile uint8_t R, G, B, F, P, W;
uint16_t OFFSET = 0;
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include <Wire.h>
#include <EEPROM.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"
#include "Visualizer.h"
#include "Sparkles.h"

#include "LabelMaps.h"



void receiveEvent(byte length) {
  cli();
  uint8_t i = 0;
  byte payload[length];

  while (Wire.available()) payload[i++] = Wire.read();
  Wire.flush();

  bool needSoftReset = false;

  for (i = 0; i < length; i += 2) {
    char label = payload[i];
    uint8_t value = payload[i + 1];

    //Serial.print(label);
    //Serial.print("=");
    //label == 'm' ? Serial.print((char)value) : Serial.print(value);
    //Serial.print("; ");

    switch (label) {
      case 'm': {
          int8_t modeIndex = label2modeIndex(value);
          if (modeIndex > -1) {
            EEPROM.write(e_mode, modeIndex);
            needSoftReset = true;
            mode = modes[modeIndex];
          }
          FFT.begin();
          break;
        }
      case 'p': {
          if (value > MAX_SPARKLES || value < 5) break;
          if (value > P) {
            if (mode == monoDiscrete || mode == monoContinuous) mleds.populate(P);
            if (mode == rgbContinuous) rgbing.populate(P);
          }
          else {
            if (mode == monoDiscrete || mode == monoContinuous) mleds.terminate(P);
            if (mode == rgbContinuous) rgbing.terminate(P);
          }
          P = value;
          EEPROM.write(e_per, P);
          break;
        }
      case 'r': R = value; EEPROM.write(e_red,   R); break;
      case 'g': G = value; EEPROM.write(e_green, G); break;
      case 'b': B = value; EEPROM.write(e_blue,  B); break;
      case 'f': F = value; EEPROM.write(e_freq,  F); break;
      case 'w': W = value; EEPROM.write(e_width, W); break;
      default: break;
    }
  }

  Serial.println();
  Serial.flush();

  if (needSoftReset) ((void (*)())NULL)();
  //  else if (!EEPROM.read(e_mode)) mode();
  sei();
}


void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  uint8_t EEPROM_RAW[e_STORED_ARGM_NUM];
  for (uint8_t i = 0; i < e_STORED_ARGM_NUM; i++) EEPROM_RAW[i] = EEPROM.read(i);

  // Load user settings
  R = EEPROM_RAW[e_red];
  G = EEPROM_RAW[e_green];
  B = EEPROM_RAW[e_blue];
  F = EEPROM_RAW[e_freq];
  P = EEPROM_RAW[e_per];
  W = EEPROM_RAW[e_width];

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  FFT.begin();  // Init Audio-Spectrum shield
  mode = modes[EEPROM.read(e_mode)]; // Load display mode

  if (mode == monoDiscrete || mode == monoContinuous) mleds.populate(P);
  if (mode == rgbContinuous) rgbing.populate(P);

  ledsetup();
  //    showColor(0, 0, 0); //blank the display
}

void loop() {
  mode();
}
