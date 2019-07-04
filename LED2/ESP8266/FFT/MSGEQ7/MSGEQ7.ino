#include "MSGEQ7.h"

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NUMPIXELS 60
#define PIN       2

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


String val2block(uint8_t val) {
  if (val > 223) return "▇";
  if (val > 191) return "▆";
  if (val > 159) return "▅";
  if (val > 127) return "▄";
  if (val > 95)  return "▃";
  if (val > 63)  return "▂";
  if (val > 31)  return "▁";
  return " ";
}

void setup() {
  FFT.begin();
  Serial.begin(115200);
  //    analogReference(INTERNAL);
  ring.begin();
  ring.clear();
  ring.show();
}

void loop() {
  FFT.read();

  for (uint8_t channel = 0; channel < 1; channel++) {
    for (uint8_t band = 0; band < BANDS; band++) {
      uint8_t level = FFT.lvls[channel][band] >> 2;
      ring.setPixelColor(band, ring.Color(level, 0, 0));
      //      Serial.print(val2block(level));
      //      Serial.print(" ");
    }
  }
  ring.show();

}
