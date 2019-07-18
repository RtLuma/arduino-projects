/*
 * EEPROM Read
 *
 * Reads the value of each byte of the EEPROM and prints it
 * to the computer.
 * This example code is in the public domain.
 */

#include <EEPROM.h>
#include <Wire.h>
#define EEPROM_ADDR 0x50           // I2C Buss address of 24LC256 256K EEPROM

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;

void setup() {
  // initialize serial and wait for port to open:
  Serial.begin(115200);
  Wire.begin();
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  for (int i=0; i < 4; i++) {
    Serial.print(EEPROM.read(0x03F6+i), DEC); Serial.print(" ");
  }
  Serial.println("Reading extEEPROM...");

  for (int i=0; i<34; i++){            // loop for first 20 slots
      Serial.print(i); Serial.print(":");
      byte bite = i2c_eeprom_read_byte(EEPROM_ADDR, i);
      Serial.print(char(bite));
      Serial.print(":");
      Serial.print(bite, HEX);
      Serial.println();
      delay(10);
  }

}

byte i2c_eeprom_read_byte( int deviceaddress, unsigned int eeaddress )
  {
    byte rdata = 0xFF;
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8));    // Address High Byte
    Wire.write((int)(eeaddress & 0xFF));  // Address Low Byte
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,1);
    if (Wire.available()) rdata = Wire.read();
    return rdata;
  }


void loop() {
  // read a byte from the current address of the EEPROM
//  value = EEPROM.read(address);

  
//  address = address + 1;
//  if (address == EEPROM.length()) {
//    address = 0;
//  }

  /***
    As the EEPROM sizes are powers of two, wrapping (preventing overflow) of an
    EEPROM address is also doable by a bitwise and of the length - 1.

    ++address &= EEPROM.length() - 1;
  ***/

//  delay(500);
}
