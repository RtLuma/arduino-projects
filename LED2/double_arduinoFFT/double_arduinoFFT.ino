#include "arduinoFFT.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
  These values can be changed in order to evaluate the functions
*/

const uint16_t samples = 16; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 9000; //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;


#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

#define CHANNEL   A0
#define NUMPIXELS 60
#define PIN       2

Adafruit_NeoPixel rings[] = {
  Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUMPIXELS, PIN + 1, NEO_GRB + NEO_KHZ800)
};

double vReal[2][samples];
double vImag[2][samples];

double maxs[2][samples >> 1] = {1.0};
double vals[2][samples >> 1] = {1.0};

void setup() {
  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  Serial.begin(115200);
  Serial.println("Ready");
  analogReference(INTERNAL); // Get signal at lower volumes
  for (uint8_t C = 0; C < 2; C++) {
    rings[C].begin();
    rings[C].clear();
    rings[C].show();
  }
}

void loop() {

  microseconds = micros();
  for (int i = 0; i < samples; i++) {
    vReal[0][i] = analogRead(CHANNEL);
    vReal[1][i] = analogRead(CHANNEL + 1);
    vImag[0][i] = vImag[1][i] = 0;
    
//    Serial.print(vReal[0][i]);
//    Serial.print(" ");
//    Serial.print(vReal[1][i]);
//    Serial.println();
    
    while (micros() - microseconds < sampling_period_us) {
      //empty loop
    }
    microseconds += sampling_period_us;
  }


  for (uint8_t C = 0; C < 2; C++) {
    FFT.Windowing(vReal[C], samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
    FFT.Compute(vReal[C], vImag[C], samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal[C], vImag[C], samples); /* Compute magnitudes */
    for (uint16_t i = 0; i < (samples >> 1); i++) {
      double magn = vReal[C][i];
      if (magn > maxs[C][i]) maxs[C][i] = magn;
      maxs[C][i] *= 0.999;
      magn /= (maxs[C][i]);
      magn = pow(magn, 5);

      vals[C][i] *= 0.5;

      if (magn > vals[C][i]) vals[C][i] = (vals[C][i] + 3.0 * magn) / 4.0;


      rings[C].setPixelColor(i, rings[C].Color(vals[C][i] * 255.0, 0, 0));
    }
    rings[C].show();
  }
}
