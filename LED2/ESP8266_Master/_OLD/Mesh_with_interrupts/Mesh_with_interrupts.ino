#define nRF24L01_IRQ_PIN 5

#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
#include <MCP3XXX.h>

MCP3002 adc;
RF24 radio(4, 0); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO
RF24Network network(radio);
RF24Mesh mesh(radio, network);

#define array_size(x) (sizeof(x) / sizeof(*x))
#define MESH_MAX_ADDRESSES 16


const uint8_t arr_size = 32;
unsigned long T = 0;
volatile unsigned long intrpts = 0;


ICACHE_RAM_ATTR void nRF24L01ISR(void) { // Add node-failure threshold to remove from DHCP pool

  // Check header, should only do DHCP if it's the req/renew message type

  cli();
  mesh.update();
  mesh.DHCP();
  intrpts++;
  sei();
}

void setup() {
  pinMode(nRF24L01_IRQ_PIN, INPUT);
  attachInterrupt(nRF24L01_IRQ_PIN, nRF24L01ISR, FALLING);

  Serial.begin(115200);
  adc.begin();
  mesh.setNodeID(0);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW); // HIGH APPARENTLY INTERFERES WITH RECEIVER >:(
  radio.maskIRQ(1, 1, 0);          // Only interupt on a received payload

  radio.printDetails();

  mesh.begin();
  mesh.update();
  mesh.DHCP();
  while (millis() < 5000) yield();
}

uint8_t multicasts = 0;

void loop() {
  //      RF24NetworkHeader header(mesh.addrList[i].address, OCT);
  //  if (millis() - T > 16) {
  //    for (int i = 0; i < mesh.addrListTop; i++) {
  //            Serial.print(mesh.addrList[i].nodeID);
  //            Serial.print("/");
  //            Serial.print(mesh.addrList[i].address, OCT);
  //            if (!mesh.write(&arr_size, 'Z', 1, mesh.addrList[i].nodeID)) {
  //              Serial.print(" Failed");
  //            }
  //            if (intrpts) {
  //              Serial.print(" Interupts: ");
  //              Serial.print(intrpts);
  //              intrpts = 0;
  //            }
  //            Serial.println();
  //
  //    }


  uint8_t payload[arr_size];
  for (int i = 0; i < arr_size; i++) payload[i] = micros() + i;
  //      RF24NetworkHeader header(mesh.addrList[i].address, char(64));
  //      RF24NetworkHeader header(0100, char(64));
  RF24NetworkHeader header(00, char(64));
  //    network.multicastLevel(1);

  for (int i = 0; i < arr_size; i++) {
    network.multicast(header, payload, 10, 1);
  }
  header.type = 65;
  network.multicast(header, payload, 1, 1);
  Serial.println("multicasts");
  //      if (network.multicast(header, payload, arr_size, 1)) if (!++multicasts) Serial.println("multicasts");

  //      network.multicast(header, payload, 1, 1);//Serial.println("idkman");
  //      header.type = 64;




  //    T = millis();
  //  }
}
