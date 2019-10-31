#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define RF24_CHANNEL             96
#define RF24_MULTICAST_PIPE_NUM  0    // Slaves don't need to write, can use this for reading
#define RF24_UNIQUE_PIPE_NUM     1
#define RF24_CE_PIN              9            
#define RF24_CSN_PIN             10
#define RF24_IRQ_PIN             2
#define PIXEL_NUM                24
#define PIXEL_PIN                4

Adafruit_NeoPixel leds = Adafruit_NeoPixel(PIXEL_NUM, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x4c, 0x54, 0x49 }; // 'M' 'U' 'L' 'T' 'I'
// const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x53, 0x49, 0x43 }; // 'M' 'U' 'S' ''I' 'C'
const uint8_t unique_pipe[]    = { 0x01, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 

///////////////////////////////////////////////////////////////////////////

volatile unsigned long noacks = 0;
volatile unsigned long acks   = 0;
unsigned long t = 0;

#define PAYLOAD_SIZE 32
volatile uint8_t payload_data[PAYLOAD_SIZE] = {255};
volatile uint8_t ack_data[PAYLOAD_SIZE] = {255};
volatile uint8_t hue=0;

///////////////////////////////////////////////////////////////////////////

void setup(void) {
  Serial.begin(115200);
  Serial.println("Reset");

  leds.begin(); leds.clear(); leds.show();

  attachInterrupt(digitalPinToInterrupt(RF24_IRQ_PIN), nRF24L01ISR, LOW);
  
  radio.begin();
  radio.powerDown(); radio.powerUp();
   radio.flush_rx();
  radio.flush_tx();
  radio.begin();
  radio.setChannel(RF24_CHANNEL);
  radio.maskIRQ(1, 1, 0); // Only interrupt on payload
  radio.setPALevel(RF24_PA_HIGH);
  // radio.setDataRate( RF24_2MBPS ); // or RF24_250KBPS 
  radio.enableDynamicPayloads(); // ACK payloads are dynamic
  radio.enableAckPayload();   //allows you to include payload on ack packet
  radio.enableDynamicAck();   ////////////// dunno if slaves need this.

  radio.openReadingPipe(RF24_MULTICAST_PIPE_NUM, multicast_pipe);
  radio.openReadingPipe(RF24_UNIQUE_PIPE_NUM, unique_pipe);
  radio.setAutoAck(RF24_MULTICAST_PIPE_NUM, 0); // Don't ACK to multicasts
  radio.setAutoAck(RF24_UNIQUE_PIPE_NUM, 1);    // ACK when spoken to directly
  radio.flush_rx(); radio.flush_tx();
  radio.startListening();
  // radio.writeAckPayload(1, &t, 1); // queue ack
  
}

///////////////////////////////////////////////////////////////////////////

void nRF24L01ISR(void) {
  
  uint8_t pipe_used = NULL; // available() writes this :/
  
  while (radio.available(&pipe_used))  {
    
    switch (pipe_used) {
      
      case RF24_MULTICAST_PIPE_NUM:
        radio.read(payload_data, PAYLOAD_SIZE); 
        break;
        
      case RF24_UNIQUE_PIPE_NUM:
        radio.read(ack_data, PAYLOAD_SIZE);
        radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, unique_pipe, 1);
        hue = ack_data[0];
        break;
      
      default:
        break;
      
    }
  }
}

///////////////////////////////////////////////////////////////////////////

void loop(void) {
  
 for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {
    
    // uint16_t mag = payload_data[i];
    // int8_t hue = ack_data[i] + (i * (256/PAYLOAD_SIZE));
    
    uint16_t mag = abs(int8_t(ack_data[i]));
    int8_t hue = payload_data[i];
    
    uint8_t r = (uint16_t(rainbow(hue + 170)) * mag) >> 8;
    uint8_t g = (uint16_t(rainbow(hue + 85))  * mag) >> 8;
    uint8_t b = (uint16_t(rainbow(hue))       * mag) >> 8;
    
    // uint8_t r = mag;
    // uint8_t g = mag;
    // uint8_t b = mag;
    
    leds.setPixelColor(i, leds.Color(r, g, b));
    Serial.println(hue);
  }
  
  leds.show();
  
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}
