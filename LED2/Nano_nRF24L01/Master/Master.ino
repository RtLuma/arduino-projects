#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"


RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const uint64_t pipe = 0xE6E6E6E6E6E6; // Needs to be the same for communicating between 2 NRF24L01

#define PIXELS 24
#define STEPSIZE 10

const uint8_t STEPMAX = 256 - STEPSIZE;

uint8_t led_data[PIXELS] = {0};
const auto payload_size = (sizeof(led_data) / sizeof(*led_data));

void setup(void) {
  Serial.begin(9600);
  Serial.println(payload_size);

  radio.begin(); // Start the NRF24L01
  radio.setChannel(10);

  radio.setAutoAck(1);        // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
  //  radio.enableAckPayload();   //allows you to include payload on ack packet
  radio.maskIRQ(1, 1, 0);       //mask all IRQ triggers except for receive (1 is mask, 0 is no mask)
  radio.setPALevel(RF24_PA_HIGH); //Set power level to low, won't work well at higher levels (interfer with receiver)
  radio.setRetries(1, 1);
//  radio.setDataRate( RF24_250KBPS ); // also 1MB/s or 2MB/s


  radio.openWritingPipe(pipe); // Get NRF24L01 ready to transmit

  randomSeed(analogRead(0));

  for (uint8_t i = 0; i < PIXELS; i++) {
    led_data[i] = ((uint16_t(i) << 8) / PIXELS);
  }
}

unsigned long int counter = 0;
unsigned long int T = 0;

void loop(void) {

  for (uint8_t i = 0; i < PIXELS; i++) {
    //    bool add = random(2);
    //    if (add) {
    //      if (led_data[i] >= STEPMAX) led_data[i]-= STEPSIZE;
    //      else led_data[i]+= STEPSIZE;
    //    }
    //    else {
    //      if (led_data[i] < STEPSIZE) led_data[i]+= STEPSIZE;
    //      else led_data[i]-= STEPSIZE;
    //    }
    led_data[i] -= 2;
    //    Serial.print(val2block(led_data[i]));
  }
  //  Serial.println();

  counter++;
  radio.write(&led_data, payload_size);
  if (millis() - T > 1000) {
    Serial.println(counter);
    counter = 0;
    T = millis();
  }
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
