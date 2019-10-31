#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIXELS 60
#define PIXELPIN  4
#define RADIO_CHANNEL 96

volatile unsigned long counter = 0;
unsigned long t = 0;

volatile uint8_t led_data[PIXELS] = {0};
const auto payload_size = (sizeof(led_data) / sizeof(*led_data));

volatile uint8_t shift_data[3] = {0};
const uint8_t shift_size = 3;

//CE, CSN
RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO
Adafruit_NeoPixel leds = Adafruit_NeoPixel(PIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x53, 0x49, 0x43 }; // M U S I C
const uint8_t unique_pipe[]    = { 0x4e, 0x4f, 0x44, 0x45, 0x00 }; // N O D E + ID

void nRF24L01ISR(void) {
  counter++;
  if (radio.available(multicast_pipe))  radio.read(&led_data, payload_size); // Multicast
  // while (radio.available(unique_pipe)) radio.read(&shift_data, shift_size); // Specific
  if (radio.available(unique_pipe))  {
    uint8_t _data[] = { micros() };
    // radio.writeAckPayload(1, _data, 1);
    radio.read(&shift_data, shift_size); // Specific
  }
  // else if (radio.available(multicast_pipe))  radio.read(&led_data, payload_size); // Multicast
    
}

void setup(void) {
  Serial.begin(115200);

  attachInterrupt(digitalPinToInterrupt(2), nRF24L01ISR, FALLING);
  // radio.powerDown();
  // radio.powerUp();
  radio.begin();
  radio.flush_rx();
  radio.flush_tx();
  // radio.read();
  radio.setChannel(RADIO_CHANNEL);

  radio.maskIRQ(1, 1, 0); // Only interrupt on payload
  radio.setPALevel(RF24_PA_HIGH);
  // radio.setDataRate( RF24_2MBPS ); // RF24_250KBPS 

  radio.openReadingPipe(0, multicast_pipe); // Slaves don't need to write, we can use this pipe to read.
  radio.openReadingPipe(1, unique_pipe);
  radio.startListening();
  
  radio.enableDynamicPayloads(); // ACK payloads are dynamic
  radio.enableAckPayload();   //allows you to include payload on ack packet
  radio.enableDynamicAck();
  radio.writeAckPayload(1, unique_pipe, 1); // ACK with its ID.
  radio.setAutoAck(1, 1); // ACK when spoken to directly
  radio.setAutoAck(0, 0); // Don't ACK to multicasts

  leds.begin();
  leds.clear();
  for (uint8_t i = 0; i < PIXELS; i++) leds.setPixelColor(i, leds.Color(255, 255, 255));
  leds.show();
}

void loop(void) {
  for (uint8_t i = 0; i < PIXELS; i++) {
    //    uint8_t hue = ((uint16_t(i) << 8) / PIXELS);
    //    uint8_t r = (uint16_t(rainbow(hue + 170)) * uint16_t(led_data[i])) >> 8;
    //    uint8_t g = (uint16_t(rainbow(hue + 85))  * uint16_t(led_data[i])) >> 8;
    //    uint8_t b = (uint16_t(rainbow(hue))       * uint16_t(led_data[i])) >> 8;
    //    leds.setPixelColor(i, leds.Color(r, g, b));
    uint32_t mag = led_data[i];
    leds.setPixelColor(i, leds.Color(
        (mag * shift_data[0]) >> 8,
        (mag * shift_data[1]) >> 8,
        (mag * shift_data[2]) >> 8
      )
    );
  }
  leds.show();
  auto _t = millis();
  if (_t - t > 1000) {
    cli();
    Serial.println(counter);
    sei();
    counter = 0;
    t = _t;
  }
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
