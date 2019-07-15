#include <Wire.h>

#define disk 0x50    //Address of 24LC256 eeprom chip
char rx_byte;
uint16_t address = 0;
char buff[32] = {0};

void setup(void) {
  Serial.begin(115200);
  Wire.begin();

  unsigned int address = 0;

}

void loop() {
  if (Serial.available()) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    if (rx_byte == '\n') {
      Serial.println();
      Serial.println("Writing to EEPROM...");
      for (uint16_t i = 0; i < address; i++) {
        Serial.println(buff[i]);
        writeEEPROM(disk, i, byte(buff[i]));
      }
      Serial.println("Reading from EEPROM...");
      for (uint16_t i = 0; i < address<<2; i+=4)
        Serial.print(char(readEEPROM(disk, address)));
      Serial.println();
      address = 0;
    }
    else {
      Serial.print(rx_byte);
      buff[address] = rx_byte;
      address++;
    }
  }
}

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data )
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();

  delay(5);
}

byte readEEPROM(int deviceaddress, unsigned int eeaddress )
{
  byte rdata = 0xFF;

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(deviceaddress, 1);

  if (Wire.available()) rdata = Wire.read();

  return rdata;
}
