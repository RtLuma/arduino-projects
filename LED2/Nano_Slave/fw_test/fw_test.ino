#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include "RadioHeaders.h"
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
const uint8_t unique_pipe[]    = { 0x02, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 

///////////////////////////////////////////////////////////////////////////

#define INITIAL_PAYLOAD_SIZE 32
volatile uint8_t* payload_data = new uint8_t[INITIAL_PAYLOAD_SIZE];
volatile uint8_t* ack_data = new uint8_t[INITIAL_PAYLOAD_SIZE];
volatile size_t payload_size = INITIAL_PAYLOAD_SIZE;
// delete [] p_array;
// volatile uint8_t payload_data[PAYLOAD_SIZE] = {255};
// volatile uint8_t ack_data[PAYLOAD_SIZE] = {255};
volatile uint8_t hue=0;

///////////////////////////////////////////////////////////////////////////

void setup(void) {
  // Serial.begin(9600);
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
volatile uint8_t last_header = 0;
volatile uint8_t last_body = 0;
volatile bool chkflag = false;

volatile uint8_t  chk8  = 0;
volatile uint16_t chk16 = 0;
volatile uint32_t chk32 = 0;


void nRF24L01ISR(void) {
  
  uint8_t pipe_used = NULL; // available() writes this :/
  
  while (radio.available(&pipe_used))  {
    size_t dyn_size = radio.getDynamicPayloadSize();
    switch (pipe_used) {
      
      case RF24_MULTICAST_PIPE_NUM:
        radio.read(payload_data, dyn_size); 
        break;
        
      case RF24_UNIQUE_PIPE_NUM: {
        radio.read(ack_data, dyn_size);
        
        switch(ack_data[0]) {
          
          case uint8_t(RadioHeader::Mode) :
            radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, ack_data, 2);
            break;

          ////////
          
          case uint8_t(RadioHeader::PayloadSize) :
            payload_size = ack_data[1];
            delete [] payload_data;
            payload_data = new uint8_t[payload_size];
            delete [] ack_data;
            ack_data = new uint8_t[payload_size];
            radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, &dyn_size, 1);
            break;

          ////////
            
          case uint8_t(RadioHeader::Firmware) :
            for (uint8_t i=1; i < dyn_size; i++) {
              uint8_t byt = ack_data[i];
              chk8  += byt;
              chk16 += byt;
              chk32 += byt;  
            }
            radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, ack_data, 1);
            break;

          ////////
            
          case uint8_t(RadioHeader::FirmwareBegin) :
            chk8  = 0;
            chk16 = 0;
            chk32 = 0; 
            radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, unique_pipe, 1);
            break;
          
          ////////
          
          case uint8_t(RadioHeader::FirmwareEnd) :
            for (uint8_t i=1; i < dyn_size; i++) {
              uint8_t byt = ack_data[i];
              chk8  += byt;
              chk16 += byt;
              chk32 += byt;  
            }
            last_body = dyn_size;
            chkflag = true;
            radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, ack_data, dyn_size);
            break;  
            
          ////////
          
          case uint8_t(RadioHeader::Health) : {
            auto t = millis();
            radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, t, 4);
            break;  
          }
          
          ////////
          
          // A No-op, mostly to get the last useful ACK payload
          case uint8_t(RadioHeader::Null) :   
          default: 
            radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, unique_pipe, 1); // Report ID
            break;
          }
        }
        
        break;
      
      default:
        break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////

void loop(void) {
  
 for (uint8_t i = 0; i < payload_size; i++) {
    
    // uint16_t mag = payload_data[i];
    int8_t hue = ack_data[i] + (i * (256/payload_size));
    uint16_t mag = abs(int8_t(ack_data[i])) << 1;
    // int8_t hue = payload_data[i];
    
    // uint8_t r = (uint16_t(rainbow(hue + 170)) * mag) >> 8;
    // uint8_t g = (uint16_t(rainbow(hue + 85))  * mag) >> 8;
    // uint8_t b = (uint16_t(rainbow(hue))       * mag) >> 8;
    
    uint8_t r = mag;
    uint8_t g = mag;
    uint8_t b = mag;
    
    leds.setPixelColor(i, leds.Color(r, g, b));
  }
  
  leds.show();
  
  if (chkflag) {
    chkflag=false;
    // 8: 71, 16: 65095 32: 654919
    if ((chk8 == 71) && (chk16 == 65095) && (chk32 == 654919)) {
      for (uint8_t i = 0; i < 24; i++)
        leds.setPixelColor(i, leds.Color(255, 64, 0));
      leds.show();
      delay(5000);
    }
  }
}

uint8_t rainbow(uint8_t hue) {
  if (hue > 170) return ~(hue + (hue << 1));
  if (hue > 85) return hue + (hue << 1);
  return 0;
}