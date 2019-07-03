#include <arduinoFFT.h>
#include <MCP3XXX.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

const uint8_t samples = 32; //This value MUST ALWAYS be a power of 2

double vReal[samples];
double vImag[samples];

#define CHANNEL   A0
#define NUMPIXELS 64
#define PIXEL_PIN  4

arduinoFFT FFT = arduinoFFT();
MCP3002 adc;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

double maxs[2][samples >> 1] = {1.0};
double vals[2][samples >> 1] = {1.0};

void setup() {
  //  Serial.begin(115200);
  adc.begin();

  leds.begin();
  leds.clear();
  leds.show();

}

void loop() {

  for (uint8_t C = 0; C < 2; C++) {
    for (int i = 0; i < samples; i++)  { vReal[i] = adc.analogRead(C); vImag[i] = 0; }
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

    for (uint16_t i = 0; i < (samples >> 1); i++) {
      double magn = vReal[i];
      if (magn > maxs[C][i]) maxs[C][i] = magn;
      maxs[C][i] *= 0.995;

      vals[C][i] *= 0.9;

      magn /= (maxs[C][i]);
      magn = pow(magn, 6);


      if (magn > vals[C][i]) vals[C][i] = (vals[C][i] + magn) / 2.0;
      //        if (magn > vals[i]) vals[i] = (vals[i] + 3.0 * magn) / 4.0;

    }
  }

  uint16_t i, pixel = 0;
  for (i = 0; i < (samples >> 1); i++, pixel++) {
    uint8_t hue = ((uint16_t(i) << 8) / (samples >> 1));
    leds.setPixelColor(pixel, leds.Color(
                         rainbow(hue + 170) * vals[0][i],
                         rainbow(hue + 85)  * vals[0][i],
                         rainbow(hue)       * vals[0][i]
                       ));
  }

  for (i = (samples >> 1) - 1; i > 0; i--, pixel++) {
    uint8_t hue = ((uint16_t(i) << 8) / (samples >> 1));
    leds.setPixelColor(pixel, leds.Color(
                         rainbow(hue + 170) * vals[1][i],
                         rainbow(hue + 85)  * vals[1][i],
                         rainbow(hue)       * vals[1][i]
                       ));
  }

  leds.show();
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}
