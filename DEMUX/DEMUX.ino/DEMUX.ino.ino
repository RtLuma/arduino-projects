#define LEDS 4

#define RP 6
#define GP 5
#define BP 4
#define COUNT1 3
#define COUNT0 2

volatile byte led = 0;
volatile byte p = -1;
volatile byte tick = 1;
volatile byte output = 0;

word c = 0;
byte rgbs[LEDS][3] = { { 0 } };
bool flop = true;

void setup() {
  DDRD = (1 << RP) + (1 << GP) + (1 << BP) + (1 << COUNT1) + (1 << COUNT0);   // DDRD = B01111100;

  cli(); // stop interrupts
  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2  = 0; // initialize counter value to 0
  OCR2A = 0; // = 16000000 / (1 * 16000000) - 1 (must be <256)
  TCCR2B |= (1 << WGM21) | (0 << CS22) | (0 << CS21) | (1 << CS20);
  TIMSK2 |= (1 << OCIE2A);

  sei(); // allow interrupts

  Serial.begin(9600);
}

ISR(TIMER2_COMPA_vect) {
  TCNT2 = 0;
  if (!(OCR2A <<= 1)) {
    OCR2A = 4;
    ++led &= 3;
  }
  PORTD = (led << COUNT0) // @RGBcc@@
          | (!(OCR2A & rgbs[led][0]) << RP) // Invert output because stupid enable-low demux
          | (!(OCR2A & rgbs[led][1]) << GP)
          | (!(OCR2A & rgbs[led][2]) << BP);
}

void loop() {
  for (int i = 0; i < LEDS; i++) {
    byte* rgb = rgbs[i];
    byte _c = c + (flop ? i * 64 : 0);
    rgb[0] = rainbow(_c + 170);
    rgb[1] = rainbow(_c + 85)>>2;
    rgb[2] = rainbow(_c)>>1;

  }
  if (!++c) flop=!flop;
  delay(15);
}

//void Rainbow(unsigned int i) {
//  i %= 1530;
//  if (i > 1529) return Rainbow(i % 1530);
//  if (i > 1274) {return strand.Color(255, 0, 255 - (i % 255));   //violet -> red
//  if (i > 1019) return strand.Color((i % 255), 0, 255);         //blue -> violet
//  if (i > 764) return strand.Color(0, 255 - (i % 255), 255);    //aqua -> blue
//  if (i > 509) return strand.Color(0, 255, (i % 255));          //green -> aqua
//  if (i > 255) return strand.Color(255 - (i % 255), 255, 0);    //yellow -> green
//  return strand.Color(255, i, 0);                               //red -> yellow
//}


byte rainbow(byte i) {

  //...126 127 -128 -127 -126...
//  if (i > 85) {
//    Serial.println(abs(char(i*3))*2, DEC);
//    delay(25);    
//    return (abs(char((i-85)*3)))<<1;
//  }
  if (i > 170) return 255 - i * 3;
  if (i > 85)  return i * 3;
  return 0;
}
