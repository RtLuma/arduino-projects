#define PIXELS 298  // Number of pixels in the string
#define PIXELS2 298
#define ALLPIXELS PIXELS + IRIS
#define SCLERA PIXELS
#define IRIS 194
#define PUPIL 104

#define PIXEL_PORT  PORTB  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRB   // Port of the pin the pixels are connected to
#define PIXEL_BIT   0      // Bit of the pin the pixels are connected to
#define PIXEL_BIT2   1     // Bit of the pin the second pixels are connected to
#define T1H  850    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns
#define T0H  350    // Width of a 0 bit in ns
#define T0L  800    // Width of a 0 bit in ns
#define RES 6000    // Width of the low gap between bits to cause a frame to latch
#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )
#define sendPixel1 sendPixel
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
inline void send2Bits( bool bitVal ) {
  if (  bitVal ) asm volatile (
      "sbi %[port], %[bit1] \n\t"
      "sbi %[port], %[bit2] \n\t"
      ".rept %[onCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit1] \n\t"
      "cbi %[port], %[bit2] \n\t"
      ".rept %[offCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit1]   "I" (PIXEL_BIT),
      [bit2]   "I" (PIXEL_BIT2),
      [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),
      [offCycles]   "I" (NS_TO_CYCLES(T1L) - 2)
    );
  else asm volatile (
      "sbi %[port], %[bit1] \n\t"
      "sbi %[port], %[bit2] \n\t"
      ".rept %[onCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit1] \n\t"
      "cbi %[port], %[bit2] \n\t"
      ".rept %[offCycles] \n\t"
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit1]   "I" (PIXEL_BIT),
      [bit2]   "I" (PIXEL_BIT2),
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
inline void send2Bytes( unsigned char byte ) {
  for ( unsigned char bit = 0 ; bit < 8 ; bit++ ) {
    send2Bits( bitRead( byte , 7 ) );
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
inline void send2Pixels( unsigned char r, unsigned char g , unsigned char b )  {
  cli(); send2Bytes(g); send2Bytes(r); send2Bytes(b); sei();
}
void show() {
  _delay_us( (RES / 1000UL) + 1);
}

void fillColor( unsigned char r , unsigned char g , unsigned char b ) {
  for ( uint16_t p = 0; p < PIXELS; p++ ) {
    sendPixel( r , g , b ); sendPixel2( r , g , b );
  }  show();
}

void fillIris( unsigned char r , unsigned char g , unsigned char b ) {
  for ( uint16_t p = 0; p < PIXELS; p++ ) {
    sendPixel2( r , g , b );
  }  show();
}

void fillSclera( unsigned char r , unsigned char g , unsigned char b ) {
  for ( uint16_t p = 0; p < PIXELS; p++ ) {
    sendPixel( r , g , b );
  }  show();
}

void theaterChase() {
  OFFSET += F;
  uint8_t l = (PIXELS + (OFFSET >> 4)) % W;
  uint16_t p;

  for (p = 0; p < SCLERA; p += W) {
    uint8_t i = 0;
    for (i; i < l; i++) sendPixel(0, 0, 0);
    sendPixel(R, G, B);
    for (++i; i < W; i++) sendPixel(0, 0, 0);
  }

  for (p = 0; p < IRIS; p += W) {
    uint8_t i = 0;
    for (i; i < W - l - 1; i++) sendPixel2(0, 0, 0);
    sendPixel2(R, G, B);
    for (++i; i < W; i++) sendPixel2(0, 0, 0);
  }

//  for (p = 0; p < PUPIL; p += W) {
//    uint8_t i = 0;
//    for (i; i < l; i++) sendPixel2(0, 0, 0);
//    sendPixel2(R, G, B);
//    for (++i; i < W; i++) sendPixel2(0, 0, 0);
//  }
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}
//p.r = rainbow(_t);
//p.g = rainbow(_t - 85);
//p.b = rainbow(_t - 170);

uint8_t rainbow16(uint16_t hue) {
  if (hue > (170 << 8)) return ~((hue + (hue << 1)) >> 8);
  if (hue > (85 << 8)) return (hue + (hue << 1)) >> 8;
  return 0;
}

void rainbowSolid(void) {
  OFFSET += F;
  uint8_t t = OFFSET >> 4;
  uint8_t r = rainbow(t),
          g = rainbow(t - 85),
          b = rainbow(t - 170);
  for ( uint16_t p = 0; p < PIXELS; p++ ) sendPixel(r, g, b);
  for ( uint16_t p = 0; p < PIXELS2; p++ ) sendPixel2(r, g, b);
}

void rainbowGradient(void) {
  OFFSET += F;
  uint8_t t = OFFSET >> 4;
  for ( uint16_t p = 0; p < SCLERA; p++ ) {
    uint8_t h = map(p, 0, SCLERA, 0, 255) + t;
    sendPixel(rainbow(h), rainbow(h - 85), rainbow(h - 170));
  }
  for ( uint16_t p = 0; p < IRIS; p++ ) {
    uint8_t h = map(p, 0, IRIS, 0, 255) + t;
    sendPixel2(rainbow(h), rainbow(h - 85), rainbow(h - 170));
  }
  for ( uint16_t p = 0; p < PUPIL; p++ ) {
    uint8_t h = map(p, 0, PUPIL, 0, 255) + t;
    sendPixel2(rainbow(h), rainbow(h - 85), rainbow(h - 170));
  }
}
