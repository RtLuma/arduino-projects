#ifdef __AVR__
#include <avr/power.h>
#endif
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
    
    uint8_t rainbow(uint8_t hue) {
        if (hue > 170) return ~(hue + (hue << 1));
        if (hue > 85) return hue + (hue << 1);
        return 0;
        }
}