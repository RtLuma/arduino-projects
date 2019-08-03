#define SERIAL_DEBUG_FRAGMENTATION
#define IF_SERIAL_DEBUG_FRAGMENTATION
#define DEBUG


#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include "printf.h"
#include <EEPROM.h>
#include <MCP3XXX.h>
MCP3002 adc;
RF24 radio(4, 0); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO
RF24Network network(radio);
RF24Mesh mesh(radio, network);

#define array_size(x) (sizeof(x) / sizeof(*x))

void setup() {
  Serial.begin(115200);
  adc.begin();

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  // Connect to the mesh
  //  radio.begin();

  //  radio.setAutoAck(0);        // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
  radio.setPALevel(RF24_PA_HIGH); //Set power level to low, won't #define IF_SERIAL_DEBUG_FRAGMENTATIONwork well at higher levels (interfer with receiver)
  //  radio.setDataRate( RF24_2MBPS );
  //  radio.printDetails();
  mesh.begin();
  //  radio.printDetails();

}

uint32_t displayTimer = 0;

void loop() {

  // Call mesh.update to keep the network updated
  mesh.update();

  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();


  // Check for incoming data from the sensors
  if (network.available()) {
    RF24NetworkHeader header;
    network.peek(header);
    Serial.print("Got ");
    uint32_t dat = 0;
    switch (header.type) {
      case 'M':
        network.read(header, &dat, sizeof(dat));
        Serial.print(dat);
        Serial.print(" from RF24Network address 0");
        Serial.println(header.from_node, OCT);
        break;
      default:
        network.read(header, 0, 0);
        Serial.println(header.type);
        break;
    }
  }


  // Send each node a message every five seconds
  // Send a different message to node 1, containing another counter instead of millis()
  if (millis() - displayTimer > 2000) {

    const uint8_t arr_size = 128;  // "random" number
    uint8_t payload[arr_size];
    for (int i = 0; i < arr_size; i++) payload[i] = micros() + i;

  for (int i = 0; i < mesh.addrListTop; i++) {

    //      RF24NetworkHeader header(mesh.addrList[i].address, OCT);
    Serial.println(mesh.write(&arr_size, 'Z', 1, mesh.addrList[i].address) ? "size OK" : "size Fail");
      Serial.println(mesh.write(payload, 'P', arr_size, mesh.addrList[i].address) ? "payload OK" : "payload Fail");

      // Serial.println( network.write(header, payload, DATA_SIZE) ? "Send OK" : "Send Fail"); //network-write instead

    }
    displayTimer = millis();
  }
}
