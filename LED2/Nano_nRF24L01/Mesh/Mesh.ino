
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

/**
   User Configuration: nodeID - A unique identifier for each radio. Allows addressing
   to change dynamically with physical changes to the mesh.

   In this example, configuration takes place below, prior to uploading the sketch to the device
   A unique value from 1-255 must be configured for each node.
   This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.

 **/
#define nodeID 3
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
  pinMode(LED_BUILTIN, OUTPUT);


  Serial.println(F("Connecting to the mesh..."));
  radio.begin();
  mesh.begin();
  Serial.println(F("Rad io"));
  radio.printDetails();

}

#define DATA_SIZE 10

bool new_data = false;
uint8_t data[DATA_SIZE];

void loop() {

  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1500) {
    //    displayTimer = millis() + 500;

    mesh.renewAddress();
    //    Serial.println('.');

    //    if (new_data) {

    //      new_data = false;
    //    }

  }

  while (network.available()) {
    RF24NetworkHeader header;

    network.read(header, &data, DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; i++) {
      Serial.print(data[i]);
      Serial.print(" ");
    }
    Serial.println();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//    if (!mesh.write(&data, 66, data, 00)) Serial.println("no dicks");
    //    new_data = true;
    break;
  }
}
