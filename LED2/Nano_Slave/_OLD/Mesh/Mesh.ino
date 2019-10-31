#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>

#define nodeID 1
#define MESH_NOMASTER 0
#define MESH_MAX_CHILDREN 3


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(9, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

volatile bool WDTtriggered = false;


ISR(TIMER1_COMPA_vect) {
  //  cli(); // stop interrupts
  WDTtriggered = true;
  //  sei(); // allow interrupts
}

void setup() {

  Serial.begin(115200);
  network.multicastRelay=true;
  mesh.setNodeID(nodeID);


  Serial.println(("Connecting 2 the mesh..."));
  mesh.begin();
  //  mesh.renewAddress();
  Serial.println(("Meshed"));

  //  auto _t = millis();
  //  while (millis() - _t < 10000) {
  //    Serial.println("Renewing...");
  //    mesh.renewAddress();
  //    if (mesh.checkConnection()) break;
  //  }
  radio.setPALevel(RF24_PA_HIGH);
  attachInterrupt(digitalPinToInterrupt(2), nRF24L01ISR, FALLING);
  //    radio.setDataRate( RF24_2MBPS );
  //    radio.setDataRate( RF24_250KBPS );

  /* http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html
    TIMER 1 for interrupt frequency 0.23842221713588158999 Hz: */
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  OCR1A = 65535; // = 16 000 000 / (1024 * 0.2385022819898341) - 1 (must be <65536)
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1 << CS12) | (0 << CS11) | (1 << CS10); // Set CS12, CS11 and CS10 bits for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei(); // allow interrupts

}

uint8_t arr_size = 32;

void nRF24L01ISR(void) {
  while (network.available()) {
    TCNT1 = 0; // reset WDT on network activity

    RF24NetworkHeader header;
    network.peek(header);
    switch (header.type) {

      case 'Z':
        network.read(header, &arr_size, 1);
        Serial.print("Got arr_length of ");
        Serial.println(arr_size);
        break;
      case 'P': {
          uint8_t payload[arr_size];
          network.read(header, payload, arr_size); //no & on payload cuz already a pointer
          uint8_t chksum = 0;
          for (int i = 0; i < arr_size - 1; i++) chksum += payload[i];
          if (chksum != payload[arr_size - 1]) {
            Serial.print("Corrupted! ");
            Serial.print(chksum);
            Serial.print(" =/= ");
            Serial.print(payload[arr_size - 1]);
            Serial.println();
          }

          break;
        }
      default:
        network.read(header, 0, 0);
        Serial.println(header.type);
        break;
    }
  }
}

void loop() {
  mesh.update();

  //Do function pointer jumper instead of logic check
  if (WDTtriggered) {
    WDTtriggered = false;
    Serial.println("watchdog triggered");
//    if (!mesh.checkConnection()) {
      mesh.renewAddress();
//      Serial.println("renew triggered");
//    }
  }
}
