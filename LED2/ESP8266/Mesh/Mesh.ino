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

const byte big_data[] = { 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69, 69 };
const auto big_size = (sizeof(big_data) / sizeof(*big_data));


struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

uint32_t ctr=0;


#define DATA_SIZE 20

bool new_data = false;
uint8_t data[DATA_SIZE];

void setup() {
  Serial.begin(115200);
  adc.begin();

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  radio.begin();

  //  radio.setAutoAck(0);        // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
  radio.setPALevel(RF24_PA_HIGH); //Set power level to low, won't #define IF_SERIAL_DEBUG_FRAGMENTATIONwork well at higher levels (interfer with receiver)
  //  radio.setDataRate( RF24_2MBPS );
  radio.printDetails();
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
  if(network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    Serial.print("Got ");
    uint32_t dat=0;
    switch(header.type){
      // Display the incoming millis() values from the sensor nodes
    case 'M': 
      network.read(header,&dat,sizeof(dat));
      Serial.print(dat);
      Serial.print(" from RF24Network address 0");
      Serial.println(header.from_node, OCT);
      break;
    default: 
      network.read(header,0,0); 
      Serial.println(header.type);
      break;
    }
  }
 
 
  // Send each node a message every five seconds
  // Send a different message to node 1, containing another counter instead of millis()
  if(millis() - displayTimer > 5000){
    ctr++;
    for (int i = 0; i < mesh.addrListTop; i++) {
      uint8_t payload[DATA_SIZE];
      for (int i=0; i < DATA_SIZE; i++) payload[i] = micros();
      RF24NetworkHeader header(mesh.addrList[i].address, OCT); //Constructing a header
      Serial.println( network.write(header, payload, DATA_SIZE) ? "Send OK" : "Send Fail"); //Sending an message
      
    }
    displayTimer = millis();
  }
}
