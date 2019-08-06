
/** RF24Mesh_Example.ino by TMRh20

   This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
   master node.
   The nodes will refresh their network address as soon as a single write fails. This allows the
   nodes to change position in relation to each other and the master node.
*/


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(9, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

#define nodeID 1
#define MESH_NOMASTER 0


uint32_t displayTimer = 0;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void setup() {

  Serial.begin(115200);
  mesh.setNodeID(nodeID);


  Serial.println(("Connecting 2 the mesh..."));
  mesh.begin();
  Serial.println(("Meshed"));
  //  mesh.renewAddress();
  //  auto _t = millis();
  //  while (millis() - _t < 10000) {
  //    Serial.println("Renewing...");
  //    mesh.renewAddress();
  //    if (mesh.checkConnection()) break;
  //  }
  radio.setPALevel(RF24_PA_HIGH);
  attachInterrupt(digitalPinToInterrupt(2), nRF24L01ISR, FALLING);
  //  radio.setDataRate( RF24_250KBPS );

}

volatile uint8_t arr_size = 32;
//volatile uint8_t payload[32] = {0};

void nRF24L01ISR(void) {
  while (network.available()) {
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
}
