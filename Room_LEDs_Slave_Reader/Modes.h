extern cRGB p;

void rainbowWave(WS2812& LED) {
  uint16_t t = millis() >> 3;
  for ( uint16_t i = 0; i < PIXELS; i++ ) {
    uint16_t _t = t + (i);
    p.r = rainbow(_t);
    p.g = rainbow(_t - 85);
    p.b = rainbow(_t - 170);
    LED.set_crgb_at(i, p);
  }
  LED.sync();
  delay(1);
}

void rainbowFill(WS2812& LED) {
  uint16_t t = millis() >> 3;
  p.r = rainbow(t);
  p.g = rainbow(t - 85);
  p.b = rainbow(t - 170);
  fillColor(LED, p);
  delay(1);
}

