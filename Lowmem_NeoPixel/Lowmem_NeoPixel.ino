#include <Wire.h>

#define PIXELS 600  // Number of pixels in the string


#define PIXEL_PORT  PORTB  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRB   // Port of the pin the pixels are connected to
#define PIXEL_BIT   0      // Bit of the pin the pixels are connected to

// These are the timing constraints taken mostly from the WS2812 datasheets
// These are chosen to be conservative and avoid problems rather than for maximum throughput

#define T1H  900    // Width of a 1 bit in ns
#define T1L  600    // Width of a 1 bit in ns

#define T0H  400    // Width of a 0 bit in ns
#define T0L  900    // Width of a 0 bit in ns

#define RES 6000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays

#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives

#define CYCLES_PER_SEC (F_CPU)

#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )

#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )

// Actually send a bit to the string. We must to drop to asm to enusre that the complier does
// not reorder things and make it so the delay happens in the wrong place.

inline void sendBit( bool bitVal ) {

  if (  bitVal ) {        // 0 bit

    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"                                // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T1H) - 2),    // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
      [offCycles]   "I" (NS_TO_CYCLES(T1L) - 2)     // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness

    );

  } else {          // 1 bit

    // **************************************************************************
    // This line is really the only tight goldilocks timing in the whole program!
    // **************************************************************************


    asm volatile (
      "sbi %[port], %[bit] \n\t"        // Set the output bit
      ".rept %[onCycles] \n\t"        // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
      "nop \n\t"                                              // Execute NOPs to delay exactly the specified number of cycles
      ".endr \n\t"
      "cbi %[port], %[bit] \n\t"                              // Clear the output bit
      ".rept %[offCycles] \n\t"                               // Execute NOPs to delay exactly the specified number of cycles
      "nop \n\t"
      ".endr \n\t"
      ::
      [port]    "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT),
      [onCycles]  "I" (NS_TO_CYCLES(T0H) - 2),
      [offCycles] "I" (NS_TO_CYCLES(T0L) - 2)

    );

  }

  // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
  // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
  // This has thenice side effect of avoid glitches on very long strings becuase


}


inline void sendByte( unsigned char byte ) {

  for ( unsigned char bit = 0 ; bit < 8 ; bit++ ) {

    sendBit( bitRead( byte , 7 ) );                // Neopixel wants bit in highest-to-lowest order
    // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
    byte <<= 1;                                    // and then shift left so bit 6 moves into 7, 5 moves into 6, etc

  }
}

void ledsetup() {
  bitSet( PIXEL_DDR , PIXEL_BIT );
}

inline void sendPixel( unsigned char r, unsigned char g , unsigned char b )  {

  sendByte(g);          // Neopixel wants colors in green then red then blue order
  sendByte(r);
  sendByte(b);

}


// Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame

void show() {
  _delay_us( (RES / 1000UL) + 1);       // Round up since the delay must be _at_least_ this long (too short might not work, too long not a problem)
}


/*

  That is the whole API. What follows are some demo functions rewriten from the AdaFruit strandtest code...

  https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino

  Note that we always turn off interrupts while we are sending pixels becuase an interupt
  could happen just when we were in the middle of somehting time sensitive.

  If we wanted to minimize the time interrupts were off, we could instead
  could get away with only turning off interrupts just for the very brief moment
  when we are actually sending a 0 bit (~1us), as long as we were sure that the total time
  taken by any interrupts + the time in our pixel generation code never exceeded the reset time (5us).

*/


// Display a single color on the whole string

void showColor( unsigned char r , unsigned char g , unsigned char b ) {

  cli();
  for ( int p = 0; p < PIXELS; p++ ) {
    sendPixel( r , g , b );
  }
  sei();
  show();

}

// Fill the dots one after the other with a color
// rewrite to lift the compare out of the loop
void colorWipe(unsigned char r , unsigned char g, unsigned char b, unsigned  char wait ) {
  for (unsigned int i = 0; i < PIXELS; i += (PIXELS / 60) ) {

    //    cli();
    unsigned int p = 0;

    while (p++ <= i) {
      sendPixel(r, g, b);
    }

    while (p++ <= PIXELS) {
      sendPixel(0, 0, 0);

    }

    //    sei();
    show();
    delay(wait);
  }
}

// Theatre-style crawling lights.
// Changes spacing to be dynmaic based on string size

#define THEATER_SPACING (PIXELS/20)

void theaterChase( unsigned char r , unsigned char g, unsigned char b, unsigned char wait ) {

  for (int j = 0; j < 3 ; j++) {

    for (int q = 0; q < THEATER_SPACING ; q++) {

      unsigned int step = 0;

      cli();

      for (int i = 0; i < PIXELS ; i++) {

        if (step == q) {

          sendPixel( r , g , b );

        } else {

          sendPixel( 0 , 0 , 0 );

        }

        step++;

        if (step == THEATER_SPACING) step = 0;

      }

      sei();

      show();
      delay(wait);

    }

  }

}



// I rewrite this one from scrtach to use high resolution for the color wheel to look nicer on a *much* bigger string

void rainbowCycle(unsigned char frames , unsigned int frameAdvance, unsigned int pixelAdvance, uint16_t wait ) {

  // Hue is a number between 0 and 3*256 than defines a mix of r->g->b where
  // hue of 0 = Full red
  // hue of 128 = 1/2 red and 1/2 green
  // hue of 256 = Full Green
  // hue of 384 = 1/2 green and 1/2 blue
  // ...

  unsigned int firstPixelHue = 0;     // Color for the first pixel in the string

  for (unsigned int j = 0; j < frames; j++) {

    unsigned int currentPixelHue = firstPixelHue;

    //    cli();

    for (unsigned int i = 0; i < PIXELS; i++) {

      if (currentPixelHue >= (3 * 256)) {              // Normalize back down incase we incremented and overflowed
        currentPixelHue -= (3 * 256);
      }

      unsigned char phase = currentPixelHue >> 8;
      unsigned char step = currentPixelHue & 0xff;

      switch (phase) {

        case 0:
          sendPixel( ~step , step ,  0 );
          break;

        case 1:
          sendPixel( 0 , ~step , step );
          break;

        case 2:
          sendPixel(  step , 0 , ~step );
          break;

      }

      currentPixelHue += pixelAdvance;


    }

    //    sei();

    show();

    firstPixelHue += frameAdvance;

    delay(wait);
  }
}


// I added this one just to demonstrate how quickly you can flash the string.
// Flashes get faster and faster until *boom* and fade to black.

void detonate( unsigned char r , unsigned char g , unsigned char b , unsigned int startdelayms) {
  while (startdelayms) {

    showColor( r , g , b );      // Flash the color
    showColor( 0 , 0 , 0 );

    delay( startdelayms );

    startdelayms =  ( startdelayms * 4 ) / 5 ;           // delay between flashes is halved each time until zero

  }

  // Then we fade to black....

  for ( int fade = 256; fade > 0; fade-- ) {

    showColor( (r * fade) / 256 , (g * fade) / 256 , (b * fade) / 256 );

  }

  showColor( 0 , 0 , 0 );


}

void setup() {

  ledsetup();

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void receiveEvent(int howMany) {
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
  }
  nextPattern();
}

typedef void (*SimplePatternList[])();

SimplePatternList gPatterns = {
  []() { theaterChase(  255,   50, 0, 10); },
  []() { showColor(125, 125, 125); delay(1);},
  []() { colorWipe(255, 0, 127, 5); },
  []() { rainbowCycle(1000 , 20 , 5, 20); },
};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

void nextPattern() {
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}



void loop() {
  gPatterns[gCurrentPatternNumber]();

  // Some example procedures showing how to display to the pixels:

//  showColor(125, 125, 125);
//  delay(5000);
//
//  colorWipe(255, 32, 0, 5); // Red
//  colorWipe(0, 255, 0, 5); // Green
//  colorWipe(0, 0, 255, 5); // Blue
//  //    colorWipe(200, 200, 200, 500);
//
//  // Send a theater pixel chase in...
//  theaterChase(127, 127, 127, 20); // White
//  //    show();
//  //    theaterChase(127,   0,   0, 5); // Red
//  theaterChase(  0,   64, 127, 20); // Blue
//
//  rainbowCycle(1000 , 20 , 5, 20);
//  //  for (int i = 0; i < PIXELS; i++)  rainbowCycle(1000 , 20 , i );
//  //    detonate( 200 , 200 , 200 , 1000);
//  //delay(100);

}






