#include "arduinoFFT.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
  These values can be changed in order to evaluate the functions
*/

const uint16_t samples = 32; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 10000; //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
unsigned long microseconds;

double vReal[samples];
double vImag[samples];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

#define CHANNEL   A0
#define NUMPIXELS 60
#define PIN       3

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
double maxs[samples >> 1] = {1.0};
double vals[samples >> 1] = {1.0};

void setup()
{
  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  Serial.begin(115200);
  Serial.println("Ready");
  analogReference(INTERNAL);
  ring.begin();
  ring.clear();
  ring.show();

  
//  pinMode(A1, OUTPUT);
//  digitalWrite(A1, LOW);
  pinMode(A2, OUTPUT); digitalWrite(A2, LOW);
  pinMode(A3, OUTPUT); digitalWrite(A3, LOW);
  pinMode(A4, OUTPUT); digitalWrite(A4, LOW);
  pinMode(A5, OUTPUT); digitalWrite(A5, LOW);
  pinMode(A6, OUTPUT); digitalWrite(A6, LOW);
  pinMode(A7, OUTPUT); digitalWrite(A7, LOW);
}

void loop()
{
 
  microseconds = micros();
  for (int i = 0; i < samples; i++)
  {
    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us) {
      //empty loop
    }
    microseconds += sampling_period_us;
  }

  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
  for (uint16_t i = 0; i < (samples >> 1); i++) {
    double magn = vReal[i];
    if (magn > maxs[i]) maxs[i] = magn;
    maxs[i] *= 0.999;
    magn /= (maxs[i]);
    magn = pow(magn, 4);
    
    vals[i] *= 0.8;
    
    if (magn > vals[i]) vals[i] = (vals[i] + 3.0*magn)/4.0;
    
    
    ring.setPixelColor(i, ring.Color(vals[i]*255.0,0,0));
//    Serial.print/ln(magn);
  }
//  Serial.println();/
  ring.show();
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < (samples >> 1); i++)  
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
        break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
        break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
        break;
    }
    Serial.print(abscissa, 6);
    if (scaleType == SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
