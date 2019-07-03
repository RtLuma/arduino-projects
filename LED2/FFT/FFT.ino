// do #defines BEFORE #includes
#define LOG_OUT 1 // use the log output function
#define FHT_N 32 // set to 256 point fht

#include <FHT.h> // include the library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NUMPIXELS 60
#define PIN       3

Adafruit_NeoPixel ring = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


uint8_t maxs[FHT_N / 2] = {1};
uint8_t lvls[FHT_N / 2] = {1};

void setup() {
  Serial.begin(115200); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter - delay() and millis() killed
  //  ADCSRA = 0xe5; // set the adc to free running mode/
  //  ADMUX = 0x40; // use adc0/
  //  DIDR0 = 0x01; // turn off th/e digital input for adc0
  //  analogReference(INTERNAL);/
  ADCSRA = 0b11100101;      // set ADC to free running mode and set pre-scalar to 32 (0xe5)
  ADMUX  = 0b11000000; // {11} Internal AREF ; {0} no left adjust ; {00000} ADC0 ADC0 x200 gain
  DIDR0  = 0b00000001; // turn off the digital input for adc0

  ring.begin();
  ring.clear();
  ring.show();
}

void loop() {
  while (1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i += 2) { // save 256 samples
      while (!(ADCSRA & 0x10)); // wait for adc to be ready
      //      ADCSRA = 0xf5; // restart adc/
      ADCSRA &= 0b11101111;
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
//      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into even bins
      //
    }
    // window data, then reorder, then run, then take output
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei(); // turn interrupts back on

    for (int i = 0; i < FHT_N / 2; ++i) {

      if (maxs[i]) maxs[i]--;
      if (lvls[i]) lvls[i]--;

      uint8_t level = fht_log_out[i];
      uint32_t ratio = 256;

      if (level < maxs[i]) {
        for (uint8_t power = 0; power < 1; power++) {
          ratio *= level;
          ratio /= maxs[i];
        }
      }
      else maxs[i] = level;

      if (ratio > lvls[i]) {
        lvls[i] += ratio + (ratio << 1);
        lvls[i] >>= 2;
      }

//      Serial.print(val2block(lvls[i]));
      Serial.print(fht_log_out[i]); Serial.print(" ");

      //      ring.setPixelColor(i, ring.Color(vals[i] * 255.0, 0, 0));
      ring.setPixelColor(i, ring.Color(lvls[i], 0, 0));

    }
    ring.show();
    Serial.println();

  }
}

String val2block(uint8_t val) {
  if (val > 223) return "▇";
  if (val > 191) return "▆";
  if (val > 159) return "▅";
  if (val > 127) return "▄";
  if (val > 95)  return "▃";
  if (val > 63)  return "▂";
  if (val > 31)  return "▁";
  return " ";
}
