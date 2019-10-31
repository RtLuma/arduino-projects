#include <Adafruit_NeoPixel.h>

#define PIXEL_NUM      64
#define PIXEL_PIN      2

Adafruit_NeoPixel leds = Adafruit_NeoPixel(PIXEL_NUM, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

namespace NeoPixel {
    void begin(void) {
        leds.begin();
        leds.clear();
        leds.show();
    }
}