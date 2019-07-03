// IMPORTANT: FFT_N should be #defined as 128 in ffft.h.
#include <avr/pgmspace.h>
#include "ffft.h"
#include <math.h>

#define ADC_CHANNEL 0
#define POOL_SIZE 16

int16_t       capture[FFT_N];    // Audio capture buffer
complex_t     bfly_buff[FFT_N];  // FFT "butterfly" buffer
uint16_t      spectrum[FFT_N / 2]; // Spectrum output buffer
volatile byte samplePos = 0;     // Buffer position counter

byte
peak[8],      // Peak level of each column; used for falling dots
     dotCount = 0, // Frame counter for delaying dot-falling speed
     colCount = 0; // Frame counter for storing past column data
int
col[8][10],   // Column levels for the prior 10 frames
    minLvlAvg[8], // For dynamic adjustment of low & high ends of graph,
    maxLvlAvg[8], // pseudo rolling averages for the prior few frames.
    colDiv[8];    // Used when filtering FFT output to 8 columns

uint16_t maxs[POOL_SIZE] = {0};
uint16_t lvls[POOL_SIZE] = {0};

/*
  These tables were arrived at through testing, modeling and trial and error,
  exposing the unit to assorted music and sounds.  But there's no One Perfect
  EQ Setting to Rule Them All, and the graph may respond better to some
  inputs than others.  The software works at making the graph interesting,
  but some columns will always be less lively than others, especially
  comparing live speech against ambient music of varying genres.
*/
static const uint8_t PROGMEM
// This is low-level noise that's subtracted from each FFT output column:
noise[64] = { 6, 6, 6, 5, 3, 4, 4, 4, 3, 4, 4, 3, 2, 3, 3, 4,
              2, 1, 2, 1, 3, 2, 3, 2, 1, 2, 3, 1, 2, 3, 4, 4,
              3, 2, 2, 2, 2, 2, 2, 1, 3, 2, 2, 2, 2, 2, 2, 2,
              2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 4
            },
            // These are scaling quotients for each FFT output column, sort of a
            // graphic EQ in reverse.  Most music is pretty heavy at the bass end.
eq[64] = {
  255, 220, 218, 225, 220, 198, 147, 99, 68, 47, 33, 22, 14,  8,  4,  2,
  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

//                                                    FFT_N << 1            ~16?
void pool(uint16_t bins[], uint16_t new_bins[], const uint8_t src_size, const uint8_t target_size) {
  uint8_t bins_per = src_size / target_size;
  uint8_t new_i = 0;
  for (uint8_t i = 0; i < src_size;) {
    uint32_t avg = 0;
    for (uint8_t b = 0; b < bins_per; b++, i++) avg += bins[i];
    avg /= bins_per;
    new_bins[new_i] = avg;
    new_i++;
  }
}

void setup() {
  Serial.begin(115200);
  uint8_t i, j, nBins, binNum, *data;

  memset(peak, 0, sizeof(peak));
  memset(col , 0, sizeof(col));

  for (i = 0; i < 8; i++) {
    minLvlAvg[i] = 0;
    maxLvlAvg[i] = 512;
    nBins        = pgm_read_byte(&data[0]) + 2;
    binNum       = pgm_read_byte(&data[1]);
    for (colDiv[i] = 0, j = 2; j < nBins; j++) colDiv[i] += pgm_read_byte(&data[j]);
  }


  // Init ADC free-run mode; f = ( 16MHz/prescaler ) / 13 cycles/conversion
  ADMUX  = 0b01000000;
  ADCSRA = _BV(ADEN)  | // ADC enable
           _BV(ADSC)  | // ADC start
           _BV(ADATE) | // Auto trigger
           _BV(ADIE)  | // Interrupt enable
           _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // 128:1 / 13 = 9615 Hz
  ADCSRB = 0;                // Free run mode, no high MUX bit
  DIDR0  = 1 << ADC_CHANNEL; // Turn off digital input for ADC pin
  TIMSK0 = 0;                // Timer0 off

  sei(); // Enable interrupts
}

void loop() {
  uint8_t  i, x, L, *data, nBins, binNum, weighting, c;
  uint16_t minLvl, maxLvl;
  int      level, y, sum;

  while (ADCSRA & _BV(ADIE)); // Wait for audio sampling to finish

  fft_input(capture, bfly_buff);   // Samples -> complex #s
  samplePos = 0;                   // Reset sample counter
  ADCSRA |= _BV(ADIE);             // Resume sampling interrupt
  fft_execute(bfly_buff);          // Process complex data
  fft_output(bfly_buff, spectrum); // Complex -> spectrum

  // Remove noise and apply EQ levels
  for (x = 0; x < FFT_N / 2; x++) {
    L = pgm_read_byte(&noise[x]);
    spectrum[x] = (spectrum[x] <= L) ? 0 :
                  (((spectrum[x] - L) * (256L - pgm_read_byte(&eq[x]))) >> 8);
            Serial.print(spectrum[x]); Serial.print(" ");
//            Serial.print(val2block(spectrum[x]));
  }
    Serial.println();

//  spectrum[1] -= 35;

  uint16_t bins[POOL_SIZE];
  pool(spectrum, bins, FFT_N / 2, POOL_SIZE);

  for (x = 0; x < POOL_SIZE; x++) {
    //    Serial.print(bins[x]); Serial.print(" ");

    if (maxs[x]) maxs[x]--;
    if (lvls[x]) lvls[x]--;

    uint8_t level = bins[x];
    uint32_t ratio = 256;

    if (level < maxs[x]) {
      for (uint8_t power = 0; power < 1; power++) {
        ratio *= level;
        ratio /= maxs[x];
      }
    }
    else maxs[x] = level;

    if (ratio > lvls[x]) {
      lvls[x] += ratio + (ratio << 1);
      lvls[x] >>= 2;
    }

//            Serial.print(val2block(bins[i]));
//    Serial.print(lvls[x]); Serial.print(" ");
//        Serial.print(bins[x]); Serial.print(" ");

  }
//  Serial.println();
}

ISR(ADC_vect) { // Audio-sampling interrupt
  static const int16_t noiseThreshold = 4;
  int16_t              sample         = ADC; // 0-1023

  capture[samplePos] =
    ((sample > (512 - noiseThreshold)) &&
     (sample < (512 + noiseThreshold))) ? 0 :
    sample - 512; // Sign-convert for FFT; -512 to +511

  if (++samplePos >= FFT_N) ADCSRA &= ~_BV(ADIE); // Buffer full, interrupt off
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
