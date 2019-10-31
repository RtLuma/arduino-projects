#ifdef __AVR__
#include <avr/power.h>
#endif

#include <arduinoFFT.h>
#include <MCP3XXX.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PIXEL_NUM      64
#define PIXEL_PIN      2
#define RF24_CHANNEL   96
#define RF24_CE_PIN    4            
#define RF24_CSN_PIN   0
#define RF24_IRQ_PIN   5
#define FFT_SAMPLES    32

MCP3002 adc;
arduinoFFT FFT = arduinoFFT();
Adafruit_NeoPixel leds = Adafruit_NeoPixel(PIXEL_NUM, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x4c, 0x54, 0x49 }; // 'M' 'U' 'L' 'T' 'I'
// const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x53, 0x49, 0x43 }; // 'M' 'U' 'S' ''I' 'C'
const uint8_t unique_pipe[]    = { 0x01, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 
const uint8_t unique_pipe2[]   = { 0x02, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 
const uint8_t *unique_pipes[] = {
  unique_pipe, unique_pipe2
};

///////////////////////////////////////////////////////////////////////////

double vReal[FFT_SAMPLES];
double vImag[FFT_SAMPLES];

uint8_t fft_payload[FFT_SAMPLES] = {0};

double maxs[2][FFT_SAMPLES >> 1] = {1.0};
double vals[2][FFT_SAMPLES >> 1] = {1.0};

unsigned long t = 0;
volatile unsigned long acks = 0;


///////////////////////////////////////////////////////////////////////////

ICACHE_RAM_ATTR void nRF24L01ISR(void) { // This should only get triggered on ACKS?
    // acks++;
    
    while (radio.isAckPayloadAvailable()) {
      uint8_t _data;
      radio.read(&_data, 1);
      // Serial.println(_data);
    }
}

///////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  
  pinMode(RF24_IRQ_PIN, INPUT);
  attachInterrupt(RF24_IRQ_PIN, nRF24L01ISR, FALLING);
    // for (int8_t j = 1; j > -1; j--) {
  
  adc.begin();

  leds.begin();
  leds.clear();
  leds.show();

  radio.begin();
  radio.setChannel(RF24_CHANNEL);
  radio.maskIRQ(1, 1, 0); // Only interrupt on payload
  
  radio.enableDynamicAck(); // Give .write a bool for getting an ACK.
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  // radio.setAutoAck(0);
  
  radio.setPALevel(RF24_PA_HIGH); 
  // radio.setRetries(15, 3); // arg1*250us delay, arg2 = tries
  radio.setRetries(0, 0); // arg1*250us delay, arg2 = tries
//  radio.setDataRate( RF24_2MBPS ); // radio.setDataRate( RF24_250KBPS );

  radio.openWritingPipe(multicast_pipe);
  radio.printDetails();
}


///////////////////////////////////////////////////////////////////////////


void loop() {
  
  computeFFT();  
  
  prepareFFTPayload();  
  
  radio.writeFast(&fft_payload, FFT_SAMPLES, true); radio.txStandBy();
  
  auto _t = millis();
  
  // if (_t - t > 50) {
  if (true) {
    // radio.flush_rx();
    for (uint8_t j = 0; j < 2; j++) {
      radio.openWritingPipe(unique_pipes[j]);
      
      auto __t = millis() >> 3;
      uint8_t _data[32];
      for (uint8_t i = 0; i < 32; i++) _data[i] = (__t+=5 - (4*j));
      
      // radio.write(&hue, 1, false);
      // radio.writeBlocking(&hue, 1, 10);
      // if (radio.writeBlocking(_data, 32, 10)) {
      if (!radio.write(_data, 32, false)) {
        Serial.print("Not sent: ");
        Serial.println(_data[0]);
      }
      
      radio.txStandBy();
    
    }
    radio.openWritingPipe(multicast_pipe);
    t = _t; 
  }
}










void computeFFT(void) {
  for (uint8_t C = 0; C < 2; C++) {
    for (int i = 0; i < FFT_SAMPLES; i++)  {
      //      vReal[i] = C ? adc.analogRead(0) : analogRead(0);
      vReal[i] = adc.analogRead(C);
      vImag[i] = 0;
    }
    FFT.Windowing(vReal, FFT_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, FFT_SAMPLES, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, FFT_SAMPLES); /* Compute magnitudes */

    double ratios[FFT_SAMPLES >> 1];
    double hardmax = 0.0;

    for (uint16_t i = 0; i < (FFT_SAMPLES >> 1); i++) {
      double magn = vReal[i];

      if (magn > maxs[C][i]) maxs[C][i] = magn;
      maxs[C][i] *= 0.9995;

      magn /= (maxs[C][i]);
//      magn = pow(magn, 2);
      magn *= magn;

      if (magn > hardmax) hardmax = magn;
      ratios[i] = magn;
    }

    for (uint16_t i = 0; i < (FFT_SAMPLES >> 1); i++) {
      double magn = ratios[i] * hardmax;

      vals[C][i] *= 0.96;

      if (magn > vals[C][i]) vals[C][i] = (vals[C][i] + magn) / 2.0;
      // if (magn > vals[i]) vals[i] = (vals[i] + 3.0 * magn) / 4.0;

    }
  }
}

void prepareFFTPayload(void) {
  uint16_t i, pixel = 0;
  for (i = 0; i < (FFT_SAMPLES >> 1); i++, pixel++) {
    uint8_t hue = ((uint16_t(i) << 8) / (FFT_SAMPLES >> 1));
    leds.setPixelColor(pixel, leds.Color(
                         rainbow(hue + 170) * vals[0][i],
                         rainbow(hue + 85)  * vals[0][i],
                         rainbow(hue)       * vals[0][i]
                       ));
    fft_payload[pixel] = 255 * vals[0][i];
  }

  for (i = (FFT_SAMPLES >> 1) - 1; i > 0; i--, pixel++) {
    uint8_t hue = ((uint16_t(i - 1) << 8) / (FFT_SAMPLES >> 1));
    leds.setPixelColor(pixel, leds.Color(
                         rainbow(hue + 170) * vals[1][i],
                         rainbow(hue + 85)  * vals[1][i],
                         rainbow(hue)       * vals[1][i]
                       ));
    fft_payload[pixel] = 255 * vals[1][i];
  }
  leds.show();
}


uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}