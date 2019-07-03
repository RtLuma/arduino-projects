#include "fix_fft.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define POWER 6
#define SAMPLES (1<<POWER)
#define FREQ 10000

unsigned int sampling_period_us;
unsigned long microseconds;

int8_t real[SAMPLES], imag[SAMPLES];

#define CHANNEL   A0
#define NUMPIXELS 64
#define PIN       3

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//double maxs[samples >> 1] = {1.0};
//double vals[samples >> 1] = {1.0};

void setup() {

  Serial.begin(115200);

  ADCSRA = 0b11100101;
  ADMUX  = 0b11000000; //5th bit set to left-adjust
  //  ADMUX  = 0b00000000; //5th bit set to left-adjust
  DIDR0  = 0b00000001;
  //    analogReference(INTERNAL);

  ring.begin();
  ring.clear();
  ring.show();

}

void loop() {

  for (uint16_t i = 0; i < SAMPLES; i++)  {
    while (!(ADCSRA & 0b00010000)); //Only works in free0runnning
    ADCSRA  = 0b11110101;
    //    Serial.println(ADC);
    byte m = ADC >> 2; // Have to read this register
    real[i] = m; // Only space for high-byte of reading
    imag[i] = 0;
  }

  //  int16_t fix_fftr(int8_t f[], int16_t m, int16_t inverse);
  //  auto scal = fix_fft(real, imag, POWER, 0);
  auto scal = fix_fftr(real, POWER, 0);

  for (uint16_t i = 0; i < SAMPLES / 2; i++)  {
    //    Serial.print(real[i]); Serial.print(" ");
    Serial.print(val2block(real[i]));
  }
  Serial.println();

  //  for (uint16_t i = 0; i < (samples >> 1); i++) {
  //    double magn = vReal[i];
  //    if (magn > maxs[i]) maxs[i] = magn;
  //    maxs[i] *= 0.99;
  //
  //    vals[i] *= 0.8;
  //
  //    magn /= (maxs[i]);
  //    magn = pow(magn, 6);
  //
  //    if (magn > vals[i]) vals[i] = (vals[i] + 3.0 * magn) / 4.0;
  //
  //    uint8_t hue = ((uint16_t(i) << 8) / (samples >> 1));
  //    //    uint8_t hue = 255*vals[i];
  //
  //    ring.setPixelColor(i, ring.Color(
  //                         rainbow(hue + 170) * vals[i],
  //                         rainbow(hue + 85)  * vals[i],
  //                         rainbow(hue)       * vals[i]
  //                       ));
  //
  //    //        Serial.print(vReal[i]); Serial.print(" ");
  //  }
  //  //  Serial.println();
  //
  //  ring.show();
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}


String val2block(uint8_t val) {
  val = (int8_t(val) + 127);
  if (val > 223) return "▇";
  if (val > 191) return "▆";
  if (val > 159) return "▅";
  if (val > 127) return "▄";
  if (val > 95)  return "▃";
  if (val > 63)  return "▂";
  if (val > 31)  return "▁";
  return " ";
}
