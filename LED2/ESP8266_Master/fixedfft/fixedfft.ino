#include <arduinoFFT.h>
#include <MCP3XXX.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NUMPIXELS 64
#define PIXEL_PIN  2
#define PIXELS 24
#define SAMPLES 32

//https://github.com/joshbainbridge/blue-noise-generator/blob/master/src/main.cpp

//https://github.com/kosme/fix_fft

MCP3002 adc;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint8_t fft_payload[SAMPLES] = {0};
const auto payload_size = (sizeof(fft_payload) / sizeof(*fft_payload));

#define ADC_SAMPLES_COUNT 1000
int16_t abuf[ADC_SAMPLES_COUNT];
int16_t abufPos = 0;


void setup() {
  Serial.begin(115200);
  adc.begin();

  leds.begin();
  leds.clear();
  leds.show();

}

void loop() {

  for (int i = 0; i < samples; i++)  {
    //      vReal[i] = C ? adc.analogRead(0) : analogRead(0);
    vReal[i] = adc.analogRead(C);
    vImag[i] = 0;
  }


  leds.show();
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}
