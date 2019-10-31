#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RF24_CHANNEL   96
#define RF24_CE_PIN    4            
#define RF24_CSN_PIN   0
#define RF24_IRQ_PIN   5

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x4c, 0x54, 0x49 }; // 'M' 'U' 'L' 'T' 'I'
// const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x53, 0x49, 0x43 }; // 'M' 'U' 'S' ''I' 'C'
const uint8_t unique_pipe[]    = { 0x01, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 
const uint8_t unique_pipe2[]   = { 0x02, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 
const uint8_t *unique_pipes[] = {
  unique_pipe, unique_pipe2
};


unsigned long t = 0;
volatile unsigned long acks = 0;

ICACHE_RAM_ATTR void nRF24L01ISR(void) { // This should only get triggered on ACKS?
    // acks++;
    
    while (radio.isAckPayloadAvailable()) {
      uint8_t _data;
      radio.read(&_data, 1);
      // Serial.println(_data);
    }
}

namespace Radio {
    void begin(void) {
        pinMode(RF24_IRQ_PIN, INPUT);
        attachInterrupt(RF24_IRQ_PIN, nRF24L01ISR, FALLING);
        
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

}