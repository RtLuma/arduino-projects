#define PIXELS 300  // Number of pixels in the string
#define PIXEL_PORT  PORTB  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRB   // Port of the pin the pixels are connected to
#define PIXEL_BIT   0      // Bit of the pin the pixels are connected to
#define PIXEL_BIT2   1      // Bit of the pin the pixels are connected to
#define T1H  900    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns
#define T0H  350    // Width of a 0 bit in ns
#define T0L  800    // Width of a 0 bit in ns
#define RES 6000    // Width of the low gap between bits to cause a frame to latch
#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )
inline void sendBit( bool bitVal ) {
  if (  bitVal ) asm volatile (
      "sbi %[port], %[bit] \n\t"
      ".rept %[onCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"
      ".rept %[offCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),
      [offCycles]   "I" (NS_TO_CYCLES(T1L) - 2)
    );
  else asm volatile (
      "sbi %[port], %[bit] \n\t"
      ".rept %[onCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"
      ".rept %[offCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
    );
}
inline void sendBit2( bool bitVal ) {
  if (  bitVal ) asm volatile (
      "sbi %[port], %[bit] \n\t"
      ".rept %[onCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"
      ".rept %[offCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT2),
      [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),
      [offCycles]   "I" (NS_TO_CYCLES(T1L) - 2)
    );
  else asm volatile (
      "sbi %[port], %[bit] \n\t"
      ".rept %[onCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"
      ".rept %[offCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT2),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)
    );
}
inline void sendByte( unsigned char byte ) {
  for ( unsigned char bit = 0 ; bit < 8 ; bit++ ) {
    sendBit( bitRead( byte , 7 ) );
    byte <<= 1;
  }
}
inline void sendByte2( unsigned char byte ) {
  for ( unsigned char bit = 0 ; bit < 8 ; bit++ ) {
    sendBit2( bitRead( byte , 7 ) );
    byte <<= 1;
  }
}
void ledsetup() {
  bitSet( PIXEL_DDR , PIXEL_BIT );
  bitSet( PIXEL_DDR , PIXEL_BIT2 );
}
inline void sendPixel( unsigned char r, unsigned char g , unsigned char b )  {
  cli(); sendByte(g); sendByte(r); sendByte(b); sei();
}
inline void sendPixel2( unsigned char r, unsigned char g , unsigned char b )  {
  cli(); sendByte2(g); sendByte2(r); sendByte2(b); sei();
}
void show() {
  _delay_us( (RES / 1000UL) + 1);
}

void showColor( unsigned char r , unsigned char g , unsigned char b ) {
  for ( uint16_t p = 0; p < PIXELS; p++ ) {
    sendPixel( r , g , b ); sendPixel2( r , g , b );
  }
  show();
  show();
}

void colorWipe(unsigned char r , unsigned char g, unsigned char b, unsigned  char wait ) {
  for (uint16_t i = 0; i < PIXELS; i++ ) sendPixel(0, 0, 0);
  for (uint16_t i = 0; i < PIXELS; i += 6 ) {

    unsigned int p = 0;

    while (p++ <= i) sendPixel(r, g, b);
    //    while (p++ <= PIXELS) sendPixel(0, 0, 0);

    show();
  }
}

#define THEATER_SPACING (PIXELS/25)

void theaterChase( unsigned char r , unsigned char g, unsigned char b) {

//  for (int j = 0; j < 3 ; j++) {

    for (int q = 0; q < THEATER_SPACING ; q++) {

      unsigned int step = 0;

      for (int i = 0; i < PIXELS ; i++) {

        if (step == q) {sendPixel( r , g , b ); sendPixel2( r , g , b );}
        else {sendPixel( 0 , 0 , 0 );sendPixel2( 0 , 0 , 0 );}

        step++;
        if (step == THEATER_SPACING) step = 0;

      }
      delayMicroseconds(85);
      show();

    } 

//  }

}



// I rewrite this one from scrtach to use high resolution for the color wheel to look nicer on a *much* bigger string

void rainbowCycle(uint16_t frameAdvance, uint16_t pixelAdvance ) {

  // Hue is a number between 0 and 3*256 than defines a mix of r->g->b where
  // hue of 0 = Full red
  // hue of 128 = 1/2 red and 1/2 green
  // hue of 256 = Full Green
  // hue of 384 = 1/2 green and 1/2 blue
  // ...

  uint16_t firstPixelHue = 0;     // Color for the first pixel in the string

  for (uint16_t j = 0; j < 768; j++) {

    uint16_t currentPixelHue = firstPixelHue;

    for (uint16_t i = 0; i < PIXELS; i++) {

      byte phase = currentPixelHue >> 8;
      byte step = currentPixelHue & 0xff;

      switch (phase) {

        case 0:
          sendPixel( ~step , step ,  0 );
          sendPixel2( ~step , step ,  0 );
          break;

        case 1:
          sendPixel( 0 , ~step , step );
          sendPixel2( 0 , ~step , step );
          break;

        case 2:
          sendPixel(  step , 0 , ~step );
          sendPixel2(  step , 0 , ~step );
          break;

      }
      if ((currentPixelHue += pixelAdvance) > 768) currentPixelHue -= 768;
    }

    show();

    firstPixelHue += frameAdvance;
    delayMicroseconds(250);
    //    delay(wait);
  }
}

byte rainbow(byte hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}
//p.r = rainbow(_t);
//p.g = rainbow(_t - 85);
//p.b = rainbow(_t - 170);

