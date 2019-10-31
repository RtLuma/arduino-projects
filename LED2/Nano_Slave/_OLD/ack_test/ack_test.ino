#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"
#include "printf.h"

#define RF24_CHANNEL             96
#define RF24_MULTICAST_PIPE_NUM  0    // Slaves don't need to write, can use this for reading
#define RF24_UNIQUE_PIPE_NUM     1
#define RF24_CE_PIN              9            
#define RF24_CSN_PIN             10
#define RF24_IRQ_PIN             2

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x53, 0x49, 0x43 }; // 'M' 'U' 'S' ''I' 'C'
const uint8_t unique_pipe[]    = { 0x00, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 

///////////////////////////////////////////////////////////////////////////

volatile unsigned long noacks = 0;
volatile unsigned long acks   = 0;
unsigned long t = 0;

#define PAYLOAD_SIZE 32
volatile uint8_t payload_data[PAYLOAD_SIZE] = {0};

///////////////////////////////////////////////////////////////////////////

void nRF24L01ISR(void) {
  
  uint8_t pipe_used = NULL; // available() sets this :/
  
  while (radio.available(&pipe_used))  {
  
    radio.read(&payload_data, PAYLOAD_SIZE); 
    
    if (pipe_used == RF24_UNIQUE_PIPE_NUM) {
      
      // acks++;
      radio.flush_tx();
      
      // uint8_t chk = 0; for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) chk += payload_data[i];
      uint8_t _data = unique_pipe[2];
      radio.writeAckPayload(RF24_UNIQUE_PIPE_NUM, &_data, 1);
      
      noacks = 0;
    }
    
    else if (pipe_used == RF24_MULTICAST_PIPE_NUM) {
      noacks++;
      // noacks+=PAYLOAD_SIZE;
    }
  }
}

///////////////////////////////////////////////////////////////////////////

void setup(void) {
  Serial.begin(115200);
  Serial.println("Reset");

  attachInterrupt(digitalPinToInterrupt(RF24_IRQ_PIN), nRF24L01ISR, FALLING);
  // radio.powerDown();
  // radio.powerUp();
  radio.begin();
  
  // radio.read();
  radio.setChannel(RF24_CHANNEL);

  radio.maskIRQ(1, 1, 0); // Only interrupt on payload
  radio.setPALevel(RF24_PA_HIGH);
  // radio.setDataRate( RF24_2MBPS ); // RF24_250KBPS 

  radio.openReadingPipe(RF24_MULTICAST_PIPE_NUM, multicast_pipe);
  radio.openReadingPipe(RF24_UNIQUE_PIPE_NUM, unique_pipe);
  radio.startListening();
  radio.flush_rx();
  radio.flush_tx();
  radio.writeAckPayload(1, &t, 1); // queue junk data.
  
  radio.enableDynamicPayloads(); // ACK payloads are dynamic
  radio.enableAckPayload();   //allows you to include payload on ack packet
  radio.enableDynamicAck();
  
  
  radio.setAutoAck(1, 1); // ACK when spoken to directly
  radio.setAutoAck(0, 0); // Don't ACK to multicasts
  radio.printDetails();

}

///////////////////////////////////////////////////////////////////////////

void loop(void) {
  // auto _t = millis();
  // if (_t - t > 1000) {
  //   if (acks) {
  //     Serial.print("ACKed: ");
  //     Serial.println(acks);
  //     acks=0;
  //   }
  //   if (noacks) {
  //     Serial.print("no-ACKed: ");
  //     Serial.println(noacks);
  //     noacks=0;
  //   }
    
  //   t = _t;
  // }
}