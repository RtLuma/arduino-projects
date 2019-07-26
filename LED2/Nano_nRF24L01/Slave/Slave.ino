#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIXELS 24
#define PIXELPIN  4
#define RADIO_CHANNEL 96
volatile uint8_t led_data[PIXELS] = {0};
const auto payload_size = (sizeof(led_data) / sizeof(*led_data));


//CE, CSN
RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO
Adafruit_NeoPixel leds = Adafruit_NeoPixel(PIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

const uint64_t pipe = 0xE6E6E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01

void nRF24L01ISR(void) {
  while (radio.available())  {
    radio.read(&led_data, payload_size); // Read information from the NRF24L01
  }
}

void setup(void) {
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(2), nRF24L01ISR, FALLING);

  radio.begin(); // Start the NRF24L01
  radio.setChannel(RADIO_CHANNEL); // Up to 128?

  radio.setAutoAck(0);        // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
//  radio.enableAckPayload();   //allows you to include payload on ack packet
  radio.maskIRQ(1, 1, 0);       //mask all IRQ triggers except for receive (1 is mask, 0 is no mask)
  radio.setPALevel(RF24_PA_MAX); //Set power level to low, won't work well at higher levels (interfer with receiver)
  radio.setDataRate( RF24_2MBPS );

  radio.openReadingPipe(1, pipe); // Get NRF24L01 ready to receive
  radio.startListening(); // Listen to see if information received

  leds.begin();
  leds.clear();
  leds.show();

}

void loop(void) {
  for (uint8_t i = 0; i < PIXELS; i++) {
    uint8_t hue = ((uint16_t(i) << 8) / PIXELS);
    uint8_t r = (uint16_t(rainbow(hue + 170)) * uint16_t(led_data[i])) >> 8;
    uint8_t g = (uint16_t(rainbow(hue + 85))  * uint16_t(led_data[i])) >> 8;
    uint8_t b = (uint16_t(rainbow(hue))       * uint16_t(led_data[i])) >> 8;
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  leds.show();
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
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
