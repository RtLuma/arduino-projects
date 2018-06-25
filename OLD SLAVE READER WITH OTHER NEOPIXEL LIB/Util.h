void fillColor(WS2812& LED, byte r, byte g, byte b ) {
  cRGB p;
  for ( int i = 0; i < PIXELS; i++ ) {
    p.r = r;
    p.g = g;
    p.b = b;
    LED.set_crgb_at(i, p);
  }
  LED.sync();
}

void fillColor(WS2812& LED, cRGB p) {
  for ( int i = 0; i < PIXELS; i++ ) LED.set_crgb_at(i, p);
  LED.sync();
}

byte rainbow(byte i) {
  if (i > 170) return 255 - i * 3;
  if (i > 85)  return i * 3;
  return 0;
}
//rainbow(c - 170), rainbow(c - 85), rainbow(c)
