
// #include "nRF24L01.h"
// #include "RF24.h"
// #include "SPI.h"
// #include "RadioHeaders.h"

#include <Wire.h>
#define EEPROM_ADDR 0x50           // I2C Buss address of 24LC256 256K EEPROM

// #define RF24_CHANNEL             96
// #define RF24_MULTICAST_PIPE_NUM  0    // Slaves don't need to write, can use this for reading
// #define RF24_UNIQUE_PIPE_NUM     1
// #define RF24_CE_PIN              9            
// #define RF24_CSN_PIN             10
// #define RF24_IRQ_PIN             2
// #define PIXEL_NUM                24
// #define PIXEL_PIN                4

// RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
// const uint8_t multicast_pipe[] = { 0x4d, 0x55, 0x4c, 0x54, 0x49 }; // 'M' 'U' 'L' 'T' 'I'
// const uint8_t unique_pipe[]    = { 0x02, 0x4e, 0x4f, 0x44, 0x45 }; // id  'N' 'O' 'D' 'E' 

#define PAYLOAD_SIZE 5029

#define EEPROM_OFFSET 20000

void setup()
{
    Wire.begin();                        // join I2C bus (address optional for master)
    Serial.begin(115200);
    randomSeed(analogRead(0));
    
    Serial.print("W: "); 
    int e = EEPROM_OFFSET;
    for (int i=0; i<PAYLOAD_SIZE; i++, e++) {
        uint8_t byt = (random() << 1) ^ (0b10101010); 
        i2c_eeprom_write_byte(EEPROM_ADDR, e, byt);
        Serial.print(byt, HEX); Serial.print(" ");
        delay(5);
    }
    
    Serial.println();
    Serial.println();
    
    Serial.print("R: "); 
    e = EEPROM_OFFSET;
    for (int i=0; i<PAYLOAD_SIZE; i++, e++) {
        uint8_t byt = i2c_eeprom_read_byte(EEPROM_ADDR, e);
        delay(5);
        Serial.print(byt, HEX); Serial.print(" ");
    }
    
    Serial.println();
    Serial.println();
    
}

void loop()
{
}

void i2c_eeprom_write_byte( int deviceaddress, unsigned int eeaddress, byte data )
{
    int rdata = data;
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8));    // Address High Byte
    Wire.write((int)(eeaddress & 0xFF));  // Address Low Byte
    Wire.write(rdata);
    Wire.endTransmission();
}

// Address is a page address, 6-bit (63). More and end will wrap around
// But data can be maximum of 28 bytes, because the Wire library has a buffer of 32 bytes
void i2c_eeprom_write_page
( int deviceaddress, unsigned int eeaddresspage, byte* data, byte length )
{
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddresspage >> 8)); // Address High Byte
    Wire.write((int)(eeaddresspage & 0xFF)); // Address Low Byte
    byte c;
    for ( c = 0; c < length; c++)
    Wire.write(data[c]);
    Wire.endTransmission();
    delay(10);                           // need some delay
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

// should not read more than 28 bytes at a time!
void i2c_eeprom_read_buffer( int deviceaddress, unsigned int eeaddress, byte *buffer, int length )
{
    Wire.beginTransmission(deviceaddress);
    Wire.write((int)(eeaddress >> 8));    // Address High Byte
    Wire.write((int)(eeaddress & 0xFF));  // Address Low Byte
    Wire.endTransmission();
    Wire.requestFrom(deviceaddress,length);
    //int c = 0;
    for ( int c = 0; c < length; c++ )
    if (Wire.available()) buffer[c] = Wire.read();
}


