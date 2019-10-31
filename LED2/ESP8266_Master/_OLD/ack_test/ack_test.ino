#include <arduinoFFT.h>
#include <MCP3XXX.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PIXEL_NUM      64
#define PIXEL_PIN      2
#define RF24_CHANNEL   96
#define RF24_CE_PIN    4            
#define RF24_CSN_PIN   0
#define RF24_IRQ_PIN   5

MCP3002 adc;
arduinoFFT FFT = arduinoFFT();
Adafruit_NeoPixel leds = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x53, 0x49, 0x43 }; // 'M' 'U' 'S' ''I' 'C'
const uint8_t unique_pipe[]    = { 0x00, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 

///////////////////////////////////////////////////////////////////////////

volatile unsigned long noacks = 0;
volatile unsigned long acks   = 0;
unsigned long t = 0;

#define NOACK_SIZE 32
uint8_t noack_data[NOACK_SIZE] = {0};

#define PAYLOAD_SIZE 32
uint8_t payload_data[PAYLOAD_SIZE] = {0};

#define ACK_SIZE 1
uint8_t ack_data[ACK_SIZE] = {0};

/* -------------------------------------------- */

const uint8_t samples = 32; //This value MUST ALWAYS be a power of 2

double vReal[samples];
double vImag[samples];


///////////////////////////////////////////////////////////////////////////

ICACHE_RAM_ATTR void nRF24L01ISR(void) { // This should only get triggered on ACKS?
  while (radio.isAckPayloadAvailable()) {
  // while (radio.available()) {
    acks++;
    radio.read(&ack_data, ACK_SIZE);
    // Serial.println(ack_data[0]);
    for (uint8_t i = 0; i < ACK_SIZE; i++) {
      Serial.print(ack_data[i]);
      Serial.print(" ");
    }
    Serial.println();
    // Serial.println(_data[0]);
  }
}

///////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  
  pinMode(RF24_IRQ_PIN, INPUT);
  attachInterrupt(RF24_IRQ_PIN, nRF24L01ISR, FALLING);
  
  adc.begin();

  radio.begin();
  radio.setChannel(RF24_CHANNEL);
  radio.maskIRQ(1, 1, 0); // Only interrupt on payload
  
  
  radio.enableDynamicAck(); // Give .write a bool for getting an ACK.
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  // radio.setAutoAck(0);
  
  radio.setPALevel(RF24_PA_HIGH); 
  radio.setRetries(15, 3); // arg1*250us delay, arg2 = tries
//  radio.setDataRate( RF24_2MBPS ); // radio.setDataRate( RF24_250KBPS );

  radio.openWritingPipe(multicast_pipe);
  radio.printDetails();
}


///////////////////////////////////////////////////////////////////////////


void loop() {

  
  for (int i = 0; i < NOACK_SIZE; i++) noack_data[i] = micros() - i;
  
  radio.writeFast(&noack_data, NOACK_SIZE, true); // true for multicasts i.e. no ACK
  
  auto _t = millis();
  if (_t - t > 1000) {
    radio.flush_rx();
    
    
    radio.openWritingPipe(unique_pipe);
    
    uint8_t chk = 0;
    for (int i = 0; i < PAYLOAD_SIZE; i++) {
      payload_data[i] = micros() - i;
      chk += payload_data[i];
    }
    
    if (radio.write(&payload_data, PAYLOAD_SIZE, false)) {
      Serial.print("Sent: #");
      Serial.println(chk);
    }
    
    while (radio.isAckPayloadAvailable()) {
      acks++;
      radio.read(&ack_data, ACK_SIZE);
      for (uint8_t i = 0; i < ACK_SIZE; i++) Serial.print(ack_data[i]);
      Serial.println();
    }
    
    radio.openWritingPipe(multicast_pipe);

    t = _t; 
    
    // if (acks) {
    //   Serial.print("ACKed: ");
    //   Serial.println(acks);
    //   acks=0;
    // }
    
  }
  
}