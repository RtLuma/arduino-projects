#include <arduinoFFT.h>

#define SAMPLES 64            //Must be a power of 2
#define  xres (SAMPLES / 2)      // Total number of  columns in the display, must be <= SAMPLES/2
#define  yres 8       // Total number of  rows in the display

double vReal[SAMPLES];
double vImag[SAMPLES];
char data_avgs[xres];

int yvalue;
int displaycolumn , displayvalue;
int peaks[xres];
arduinoFFT FFT = arduinoFFT();                                    // FFT object



void setup() {
  ADCSRA = 0b11100101;      // set ADC to free running mode and set pre-scalar to 32 (0xe5)
  //  ADMUX = 0b00000000;       // use pin A0 and external voltage reference
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  Serial.begin(115200);
  analogReference(INTERNAL);
}

void loop() {
  // ++ Sampling
  for (int i = 0; i < SAMPLES; i++)  {
    while (!(ADCSRA & 0x10));
    ADCSRA = 0b11110101 ;
//    int value = ADC - 512 ;

    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // 1form into a signed int
    k <<= 6; // form into a 16b signed int

    vReal[i] = k;                   // Copy to bins after compressing
    vImag[i] = 0;
  }

  // -- Sampling


  // ++ FFT
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  // -- FFT


  // ++ re-arrange FFT result to match with no. of columns on display ( xres )
  int step = (SAMPLES / 2) / xres;
  int c = 0;
  for (int i = 0; i < (SAMPLES / 2); i += step)  {
    data_avgs[c] = 0;
    for (int k = 0 ; k < step ; k++) {
      data_avgs[c] = data_avgs[c] + vReal[i + k];
    }
    data_avgs[c] = data_avgs[c] / step;
    c++;
  }
  // -- re-arrange FFT result to match with no. of columns on display ( xres )


  // ++ send to display according measured value
  for (int i = 0; i < xres; i++)  {
    //    data_avgs[i] = constrain(data_avgs[i], 0, 80);          // set max & min values for buckets
    //    data_avgs[i] = map(data_avgs[i], 0, 80, 0, yres);        // remap averaged values to yres
    yvalue = data_avgs[i];

    peaks[i] -= 5;  // decay by one light
    if (yvalue > peaks[i]) peaks[i] = yvalue ;
    yvalue = peaks[i];
    Serial.print(val2block(yvalue));
    //    Serial.print(" ");
    //      mx.setColumn(displaycolumn, displayvalue);              // for left to right
  }
  Serial.println();
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
