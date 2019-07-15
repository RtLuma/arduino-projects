#include <24LC256.h>
#define EEPROM_ADDRESS 0x50 // 24LC256 EEPROM Address in i2c bus 

E24LC256 mem1(EEPROM_ADDRESS);

char rx_byte;
uint16_t address = 0;
char buff[32] = {0};

void setup(void) {
  Serial.begin(115200);
  if (mem1.detect()) Serial.println("EEPROM 1 detected!");
  else Serial.println("EEPROM 1 not detected!");


  unsigned int address = 0;

}

void loop() {
  if (Serial.available()) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    if (rx_byte == '\n') {
      Serial.println();
      Serial.println("Writing to EEPROM...");
      for (uint16_t i = 0; i < address; i++) {
        mem1.writeByte(i, byte(buff[i]));
      }
      Serial.println("Reading from EEPROM...");
      for (uint16_t i = 0; i < 32; i+=8) {
        for (uint16_t j = i; j < 8; j++) {
          Serial.print((mem1.readByte(j)));
          Serial.print(" ");
        }
        Serial.println();
      }
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
