#include <arduinoFFT.h>
#include <MCP3XXX.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint8_t samples = 32; //This value MUST ALWAYS be a power of 2

double vReal[samples];
double vImag[samples];

#define CHANNEL   A0
#define NUMPIXELS 64
#define PIXEL_PIN  2
#define PIXELS 24
#define RADIO_CHANNEL 96

arduinoFFT FFT = arduinoFFT();
MCP3002 adc;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
//        CE, CSN
RF24 radio(4, 0); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO
const uint64_t pipe = 0xE6E6E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01

uint8_t fft_payload[samples] = {0};
const auto payload_size = (sizeof(fft_payload) / sizeof(*fft_payload));

double maxs[2][samples >> 1] = {1.0};
double vals[2][samples >> 1] = {1.0};


void setup() {
  Serial.begin(115200);
  adc.begin();

  leds.begin();
  leds.clear();
  leds.show();

  radio.begin(); // Start the NRF24L01
  radio.setChannel(RADIO_CHANNEL);

  radio.setAutoAck(0);        // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
  //  radio.enableAckPayload();   //allows you to include payload on ack packet
  radio.setPALevel(RF24_PA_HIGH); //Set power level to low, won't work well at higher levels (interfer with receiver)
  radio.setRetries(0, 0);
  radio.setDataRate( RF24_2MBPS );
//  radio.setDataRate( RF24_250KBPS );
  radio.openWritingPipe(pipe); // Get NRF24L01 ready to transmit
  radio.printDetails();
}

void loop() {

  for (uint8_t C = 0; C < 2; C++) {
    for (int i = 0; i < samples; i++)  {
      //      vReal[i] = C ? adc.analogRead(0) : analogRead(0);
      vReal[i] = adc.analogRead(C);
      vImag[i] = 0;
    }
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

    double ratios[samples >> 1];
    double hardmax = 0.0;

    for (uint16_t i = 0; i < (samples >> 1); i++) {
      double magn = vReal[i];

      if (magn > maxs[C][i]) maxs[C][i] = magn;
      maxs[C][i] *= 0.999;

      magn /= (maxs[C][i]);
//      magn = pow(magn, 2);
      magn *= magn;

      if (magn > hardmax) hardmax = magn;
      ratios[i] = magn;
    }

    for (uint16_t i = 0; i < (samples >> 1); i++) {
      double magn = ratios[i] * hardmax;

      vals[C][i] *= 0.95;

      if (magn > vals[C][i]) vals[C][i] = (vals[C][i] + magn) / 2.0;
      //        if (magn > vals[i]) vals[i] = (vals[i] + 3.0 * magn) / 4.0;

    }
  }

  uint16_t i, pixel = 0;
  for (i = 0; i < (samples >> 1); i++, pixel++) {
    uint8_t hue = ((uint16_t(i) << 8) / (samples >> 1));
    leds.setPixelColor(pixel, leds.Color(
                         rainbow(hue + 170) * vals[0][i],
                         rainbow(hue + 85)  * vals[0][i],
                         rainbow(hue)       * vals[0][i]
                       ));
    fft_payload[pixel] = 255 * vals[0][i];
  }

  for (i = (samples >> 1) - 1; i > 0; i--, pixel++) {
    uint8_t hue = ((uint16_t(i - 1) << 8) / (samples >> 1));
    leds.setPixelColor(pixel, leds.Color(
                         rainbow(hue + 170) * vals[1][i],
                         rainbow(hue + 85)  * vals[1][i],
                         rainbow(hue)       * vals[1][i]
                       ));
    fft_payload[pixel] = 255 * vals[1][i];
  }
  radio.write(&fft_payload, payload_size);
  leds.show();
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}
