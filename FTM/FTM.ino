#define LEDS 5
const uint8_t PROGMEM PINS[] = {2, 3, 4, 5, 6, 7, 14, 15, 16, 17, 18, 19, 8, 9, 10};

word c = 0;
byte rgbs[LEDS][3] = { 0 };
byte parts[LEDS] = { 0 };

volatile uint8_t wait = 4;
volatile int16_t pulse = 0;

void setup() {
  Serial.begin(38400);
  for (auto pin : PINS) pinMode(pin, OUTPUT);
  
  byte part = 256 / LEDS;
  for (int i = 0; i < LEDS; i++) parts[i] = i * part;

  FTM0_SC = 0;    //Disable interupt register
  FTM0_CNT = 0;   //Reset counter
  FTM0_MOD = 256; //Set initial tick-amount to wait
  
  // Bits | Va1ue | Description
  //  8   |    0  | DMA: Disable DMA
  //  7   |    1  | TOF: Clear Timer Overflow Flag
  //  6   |    1  | TOIE: Enable Timer Overflow Interrupt
  //  5   |    0  | CPWMS: TPM in up counting mode
  // 4-3  |   01  | CMOD: Counter incrememnts every TPM clock
  // 2-0  |  111  | PS: Prescale = 128
  FTM0_SC = 0b011001111;
  NVIC_SET_PRIORITY(IRQ_FTM0, 64);
  NVIC_ENABLE_IRQ(IRQ_FTM0);
}

void ftm0_isr(void) {
  if (!(wait <<= 1)) {
    wait = 1;  //Wait twice as long with overflow
    pulse += 3;
  }

  PORTD =       (wait & rgbs[0][0] ? 4   : 0) |
                (wait & rgbs[0][1] ? 8   : 0) |
                (wait & rgbs[0][2] ? 16  : 0) |
                (wait & rgbs[1][0] ? 32  : 0) |
                (wait & rgbs[1][1] ? 64  : 0) |
                (wait & rgbs[1][2] ? 128 : 0)
                ;

  PORTC =       (wait & rgbs[2][0] ? 1  : 0) |
                (wait & rgbs[2][1] ? 2  : 0) |
                (wait & rgbs[2][2] ? 4  : 0) |
                (wait & rgbs[3][0] ? 8  : 0) |
                (wait & rgbs[3][1] ? 16 : 0) |
                (wait & rgbs[3][2] ? 32 : 0)
                ;

  PORTB =       (wait & rgbs[4][0] ? 1  : 0) |
                (wait & rgbs[4][1] ? 2  : 0) |
                (wait & rgbs[4][2] ? 4  : 0)
                ;

  FTM0_MOD = wait;      //Modulate timer counts to new wait-tick amount
  FTM0_SC |= (1 << 7);  //Clear interupt flag
  FTM0_CNT = 0;         //Reset timer
}


void loop() {
  for (int i = 0; i < LEDS; i++) {
    byte* rgb = rgbs[i];
    byte _c = c + parts[i];
    rgb[0] = rainbow(_c - 170);
    rgb[1] = rainbow(_c - 85);
    rgb[2] = rainbow(_c);
  }
  c--;
  delayMicroseconds(abs(pulse) / 10);
}

byte rainbow(byte i) {
  if (i > 170) return 255 - i * 3;
  if (i > 85)  return i * 3;
  return 0;
}


