#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN           12
#define NUMPIXELS     24
#define ACCEL         1
#define RAND_RANGE    3000
#define T_RESET       t=random(RAND_RANGE) + 1000
#define TERM_VELOC    300

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
extern volatile unsigned long timer0_millis;

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}
//p.r = rainbow(_t);
//p.g = rainbow(_t - 85);
//p.g = rainbow(_t - 127);
#define TERM_VELOCITY 127

int16_t v = 0;
uint16_t p = 0;
uint32_t t = 0;
int8_t a = ACCEL;
uint8_t hue;
uint8_t wait = 255;


void setup() {
  Serial.begin(9600);
  ring.begin();
  randomSeed(analogRead(0) * analogRead(1));
  T_RESET;
  p = random(NUMPIXELS);
  v = random(255);
  hue = random(255);
}

void loop() {
  if (millis() > t) {
    timer0_millis = 0;
    T_RESET;
    a *= -1;
    v += v > 0 ? -5 : 5;
//    hue = random(255);
  }
  if (abs(v) < TERM_VELOC) v += a;
  p = (p + v);
  uint8_t pos = map(p, 0, 0xFFFF, 0, 24);
  ring.setPixelColor(pos, ring.Color(rainbow(v), rainbow(v - 85), rainbow(v - 127)));

  for (int i = 0; i < NUMPIXELS; i++) {
    uint32_t col = ring.getPixelColor(i);
    uint8_t r = col >> 16;
    uint8_t g = col >> 8;
    uint8_t b = col;
    if (r>0) r*=.995;
    if (g>0) g*=.995;
    if (b>0) b*=.995;
    ring.setPixelColor(i, ring.Color(r, g, b));
//    ring.setPixelColor(i, ring.Color(0, 0, 0));
  }
  ring.setPixelColor(pos, ring.Color(rainbow(hue), rainbow(hue - 85), rainbow(hue - 127)));
  if (!--wait) hue+=5;
  ring.show();

}
