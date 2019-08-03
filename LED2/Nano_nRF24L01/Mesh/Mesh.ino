
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
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(("Connecting 2 the mesh..."));
  mesh.begin();
//  mesh.renewAddress();
}

uint8_t arr_size = 32;

void loop() {

  mesh.update();
  //  Serial.println("asses n cocks");
  //  delay(100);

  // Send to the master node every second
  if (millis() - displayTimer >= 5000) {

//    if ( ! mesh.checkConnection() ) {
//      //refresh the network address
//      Serial.println("Renewing Address");
//      mesh.renewAddress();
//      displayTimer = millis();
//    }

    //    displayTimer = millis();
    //
    //    // Send an 'M' type message containing the current millis()
    //    if (!mesh.write(&displayTimer, 'M', sizeof(displayTimer))) {
    //
    //      // If a write fails, check connectivity to the mesh network
    //      if ( ! mesh.checkConnection() ) {
    //        //refresh the network address
    //        Serial.println("Renewing Address");
    //        mesh.renewAddress();
    //      } else {
    //        Serial.println("Send fail, Test OK");
    //      }
    //    } else {
    //      Serial.print("Send OK: "); Serial.println(displayTimer);
    //    }
  }

  while (network.available()) {
    RF24NetworkHeader header;
    network.peek(header);
    switch (header.type) {

      case 'Z':
        network.read(header, &arr_size, 1);
        Serial.print("Got arr_length of ");
        Serial.println(arr_size);
        break;
      case 'P':
        uint8_t payload[arr_size];
        network.read(header, payload, arr_size); //no & on payload cuz already a pointer
//        Serial.println("Got payload:");
//        for (int i = 0; i < arr_size; i++) {
//          Serial.print(payload[i]);
//          Serial.print(" ");
//        }
//        Serial.println();
        break;
      default:
        network.read(header, 0, 0);
        Serial.println(header.type);
        break;
    }
  }
}
