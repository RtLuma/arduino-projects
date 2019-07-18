
#include <Wire.h>
#define EEPROM_ADDR 0x50           // I2C Buss address of 24LC256 256K EEPROM
const byte binData[] PROGMEM={0x41, 0x42, 0x6d, 0x69, 0x6e, 0x69, 0x62, 0x6f, 0x6f, 0x74, 0x64, 0x69, 0x63, 0x6b, 0x73, 0x33, 0xff, 0xff, 0xff, 0xff, 0x5D, 0x30, 0x13, 0xB4, 0x5D, 0x30, 0x13, 0xB4, 0x99, 0x62, 0x8b, 0x6c, 0x0a, 0x50, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x35, 0x43, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x43, 0x41, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x41, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x39, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x33, 0x30, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x38, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x34, 0x30, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x31, 0x36, 0x30, 0x31, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x43, 0x46, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x35, 0x30, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x36, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x36, 0x30, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x43, 0x39, 0x34, 0x36, 0x45, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x34, 0x30, 0x30, 0x32, 0x37, 0x30, 0x30, 0x32, 0x39, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x37, 0x30, 0x30, 0x30, 0x32, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x35, 0x30, 0x30, 0x32, 0x38, 0x30, 0x30, 0x32, 0x42, 0x30, 0x30, 0x30, 0x34, 0x30, 0x34, 0x30, 0x34, 0x30, 0x34, 0x43, 0x45, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x38, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0x34, 0x30, 0x34, 0x30, 0x34, 0x30, 0x32, 0x30, 0x32, 0x30, 0x32, 0x30, 0x32, 0x30, 0x32, 0x30, 0x32, 0x30, 0x33, 0x30, 0x33, 0x30, 0x33, 0x30, 0x33, 0x30, 0x33, 0x30, 0x33, 0x34, 0x32, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x39, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30, 0x34, 0x30, 0x38, 0x31, 0x30, 0x32, 0x30, 0x34, 0x30, 0x38, 0x30, 0x30, 0x31, 0x30, 0x32, 0x30, 0x34, 0x30, 0x38, 0x31, 0x30, 0x32, 0x30, 0x30, 0x31, 0x30, 0x32, 0x31, 0x46, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x41, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0x38, 0x31, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x30, 0x30, 0x30, 0x32, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x33, 0x30, 0x34, 0x30, 0x37, 0x46, 0x42, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x42, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x32, 0x34, 0x31, 0x46, 0x42, 0x45, 0x43, 0x46, 0x45, 0x46, 0x44, 0x38, 0x45, 0x30, 0x42, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x43, 0x30, 0x30, 0x30, 0x44, 0x45, 0x42, 0x46, 0x43, 0x44, 0x42, 0x46, 0x32, 0x31, 0x45, 0x30, 0x41, 0x30, 0x45, 0x30, 0x42, 0x31, 0x45, 0x30, 0x30, 0x31, 0x43, 0x30, 0x31, 0x44, 0x39, 0x32, 0x41, 0x39, 0x33, 0x30, 0x41, 0x43, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x44, 0x30, 0x30, 0x30, 0x42, 0x32, 0x30, 0x37, 0x45, 0x31, 0x46, 0x37, 0x30, 0x45, 0x39, 0x34, 0x36, 0x30, 0x30, 0x31, 0x30, 0x43, 0x39, 0x34, 0x43, 0x46, 0x30, 0x31, 0x30, 0x43, 0x39, 0x34, 0x30, 0x30, 0x30, 0x30, 0x37, 0x43, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x45, 0x30, 0x30, 0x30, 0x45, 0x31, 0x45, 0x42, 0x46, 0x30, 0x45, 0x30, 0x39, 0x34, 0x39, 0x31, 0x45, 0x44, 0x45, 0x39, 0x46, 0x30, 0x45, 0x30, 0x32, 0x34, 0x39, 0x31, 0x45, 0x39, 0x45, 0x38, 0x46, 0x30, 0x45, 0x30, 0x35, 0x33, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x30, 0x46, 0x30, 0x30, 0x30, 0x45, 0x34, 0x39, 0x31, 0x45, 0x45, 0x32, 0x33, 0x30, 0x39, 0x46, 0x34, 0x33, 0x43, 0x43, 0x30, 0x39, 0x39, 0x32, 0x33, 0x33, 0x39, 0x46, 0x31, 0x39, 0x33, 0x33, 0x30, 0x39, 0x31, 0x46, 0x30, 0x35, 0x37, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x33, 0x38, 0x46, 0x34, 0x39, 0x31, 0x33, 0x30, 0x41, 0x39, 0x46, 0x30, 0x39, 0x32, 0x33, 0x30, 0x30, 0x31, 0x46, 0x35, 0x39, 0x34, 0x42, 0x35, 0x39, 0x46, 0x37, 0x44, 0x31, 0x32, 0x43, 0x30, 0x37, 0x41, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x31, 0x30, 0x30, 0x30, 0x39, 0x37, 0x33, 0x30, 0x39, 0x31, 0x46, 0x30, 0x39, 0x38, 0x33, 0x30, 0x41, 0x31, 0x46, 0x30, 0x39, 0x34, 0x33, 0x30, 0x42, 0x39, 0x46, 0x34, 0x39, 0x30, 0x39, 0x31, 0x38, 0x30, 0x30, 0x30, 0x32, 0x43, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x32, 0x30, 0x30, 0x30, 0x39, 0x46, 0x37, 0x44, 0x30, 0x33, 0x43, 0x30, 0x39, 0x30, 0x39, 0x31, 0x38, 0x30, 0x30, 0x30, 0x39, 0x46, 0x37, 0x37, 0x39, 0x30, 0x39, 0x33, 0x38, 0x30, 0x30, 0x30, 0x30, 0x44, 0x43, 0x30, 0x43, 0x39, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x33, 0x30, 0x30, 0x30, 0x39, 0x34, 0x42, 0x35, 0x39, 0x46, 0x37, 0x37, 0x39, 0x34, 0x42, 0x44, 0x30, 0x39, 0x43, 0x30, 0x39, 0x30, 0x39, 0x31, 0x42, 0x30, 0x30, 0x30, 0x39, 0x46, 0x37, 0x37, 0x30, 0x33, 0x43, 0x30, 0x39, 0x43, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x34, 0x30, 0x30, 0x30, 0x39, 0x30, 0x39, 0x31, 0x42, 0x30, 0x30, 0x30, 0x39, 0x46, 0x37, 0x44, 0x39, 0x30, 0x39, 0x33, 0x42, 0x30, 0x30, 0x30, 0x46, 0x30, 0x45, 0x30, 0x45, 0x45, 0x30, 0x46, 0x46, 0x46, 0x31, 0x46, 0x30, 0x34, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x35, 0x30, 0x30, 0x30, 0x45, 0x45, 0x35, 0x38, 0x46, 0x46, 0x34, 0x46, 0x41, 0x35, 0x39, 0x31, 0x42, 0x34, 0x39, 0x31, 0x39, 0x46, 0x42, 0x37, 0x46, 0x38, 0x39, 0x34, 0x38, 0x31, 0x31, 0x31, 0x30, 0x34, 0x43, 0x30, 0x35, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x36, 0x30, 0x30, 0x30, 0x38, 0x43, 0x39, 0x31, 0x32, 0x30, 0x39, 0x35, 0x32, 0x38, 0x32, 0x33, 0x30, 0x32, 0x43, 0x30, 0x45, 0x43, 0x39, 0x31, 0x32, 0x45, 0x32, 0x42, 0x32, 0x43, 0x39, 0x33, 0x39, 0x46, 0x42, 0x46, 0x42, 0x44, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x37, 0x30, 0x30, 0x30, 0x30, 0x38, 0x39, 0x35, 0x33, 0x46, 0x42, 0x37, 0x46, 0x38, 0x39, 0x34, 0x38, 0x30, 0x39, 0x31, 0x30, 0x35, 0x30, 0x31, 0x39, 0x30, 0x39, 0x31, 0x30, 0x36, 0x30, 0x31, 0x41, 0x30, 0x39, 0x31, 0x46, 0x30, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x38, 0x30, 0x30, 0x30, 0x30, 0x37, 0x30, 0x31, 0x42, 0x30, 0x39, 0x31, 0x30, 0x38, 0x30, 0x31, 0x32, 0x36, 0x42, 0x35, 0x41, 0x38, 0x39, 0x42, 0x30, 0x35, 0x43, 0x30, 0x32, 0x46, 0x33, 0x46, 0x31, 0x39, 0x46, 0x30, 0x43, 0x33, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x39, 0x30, 0x30, 0x30, 0x30, 0x31, 0x39, 0x36, 0x41, 0x31, 0x31, 0x44, 0x42, 0x31, 0x31, 0x44, 0x33, 0x46, 0x42, 0x46, 0x42, 0x41, 0x32, 0x46, 0x41, 0x39, 0x32, 0x46, 0x39, 0x38, 0x32, 0x46, 0x38, 0x38, 0x32, 0x37, 0x30, 0x37, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x41, 0x30, 0x30, 0x30, 0x38, 0x32, 0x30, 0x46, 0x39, 0x31, 0x31, 0x44, 0x41, 0x31, 0x31, 0x44, 0x42, 0x31, 0x31, 0x44, 0x42, 0x43, 0x30, 0x31, 0x43, 0x44, 0x30, 0x31, 0x34, 0x32, 0x45, 0x30, 0x36, 0x36, 0x30, 0x46, 0x36, 0x32, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x42, 0x30, 0x30, 0x30, 0x37, 0x37, 0x31, 0x46, 0x38, 0x38, 0x31, 0x46, 0x39, 0x39, 0x31, 0x46, 0x34, 0x41, 0x39, 0x35, 0x44, 0x31, 0x46, 0x37, 0x30, 0x38, 0x39, 0x35, 0x38, 0x46, 0x39, 0x32, 0x39, 0x46, 0x39, 0x32, 0x42, 0x34, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x43, 0x30, 0x30, 0x30, 0x41, 0x46, 0x39, 0x32, 0x42, 0x46, 0x39, 0x32, 0x43, 0x46, 0x39, 0x32, 0x44, 0x46, 0x39, 0x32, 0x45, 0x46, 0x39, 0x32, 0x46, 0x46, 0x39, 0x32, 0x30, 0x45, 0x39, 0x34, 0x42, 0x39, 0x30, 0x30, 0x35, 0x45, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x44, 0x30, 0x30, 0x30, 0x34, 0x42, 0x30, 0x31, 0x35, 0x43, 0x30, 0x31, 0x38, 0x38, 0x45, 0x45, 0x43, 0x38, 0x32, 0x45, 0x38, 0x33, 0x45, 0x30, 0x44, 0x38, 0x32, 0x45, 0x45, 0x31, 0x32, 0x43, 0x46, 0x31, 0x32, 0x43, 0x37, 0x37, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x45, 0x30, 0x30, 0x30, 0x30, 0x45, 0x39, 0x34, 0x42, 0x39, 0x30, 0x30, 0x44, 0x43, 0x30, 0x31, 0x43, 0x42, 0x30, 0x31, 0x38, 0x38, 0x31, 0x39, 0x39, 0x39, 0x30, 0x39, 0x41, 0x41, 0x30, 0x39, 0x42, 0x42, 0x30, 0x39, 0x35, 0x31, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x31, 0x46, 0x30, 0x30, 0x30, 0x38, 0x38, 0x33, 0x45, 0x39, 0x33, 0x34, 0x30, 0x41, 0x31, 0x30, 0x35, 0x42, 0x31, 0x30, 0x35, 0x39, 0x38, 0x46, 0x33, 0x32, 0x31, 0x45, 0x30, 0x43, 0x32, 0x31, 0x41, 0x44, 0x31, 0x30, 0x38, 0x43, 0x39, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x30, 0x30, 0x30, 0x30, 0x45, 0x31, 0x30, 0x38, 0x46, 0x31, 0x30, 0x38, 0x38, 0x38, 0x45, 0x45, 0x38, 0x38, 0x30, 0x45, 0x38, 0x33, 0x45, 0x30, 0x39, 0x38, 0x31, 0x45, 0x41, 0x31, 0x31, 0x43, 0x42, 0x31, 0x31, 0x43, 0x35, 0x44, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x31, 0x30, 0x30, 0x30, 0x43, 0x31, 0x31, 0x34, 0x44, 0x31, 0x30, 0x34, 0x45, 0x31, 0x30, 0x34, 0x46, 0x31, 0x30, 0x34, 0x31, 0x39, 0x46, 0x37, 0x46, 0x46, 0x39, 0x30, 0x45, 0x46, 0x39, 0x30, 0x44, 0x46, 0x39, 0x30, 0x43, 0x44, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x32, 0x30, 0x30, 0x30, 0x43, 0x46, 0x39, 0x30, 0x42, 0x46, 0x39, 0x30, 0x41, 0x46, 0x39, 0x30, 0x39, 0x46, 0x39, 0x30, 0x38, 0x46, 0x39, 0x30, 0x30, 0x38, 0x39, 0x35, 0x31, 0x46, 0x39, 0x32, 0x30, 0x46, 0x39, 0x32, 0x41, 0x34, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x33, 0x30, 0x30, 0x30, 0x30, 0x46, 0x42, 0x36, 0x30, 0x46, 0x39, 0x32, 0x31, 0x31, 0x32, 0x34, 0x32, 0x46, 0x39, 0x33, 0x33, 0x46, 0x39, 0x33, 0x38, 0x46, 0x39, 0x33, 0x39, 0x46, 0x39, 0x33, 0x41, 0x46, 0x39, 0x33, 0x46, 0x39, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x34, 0x30, 0x30, 0x30, 0x42, 0x46, 0x39, 0x33, 0x38, 0x30, 0x39, 0x31, 0x30, 0x31, 0x30, 0x31, 0x39, 0x30, 0x39, 0x31, 0x30, 0x32, 0x30, 0x31, 0x41, 0x30, 0x39, 0x31, 0x30, 0x33, 0x30, 0x31, 0x42, 0x30, 0x39, 0x31, 0x41, 0x46, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x35, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0x31, 0x33, 0x30, 0x39, 0x31, 0x30, 0x30, 0x30, 0x31, 0x32, 0x33, 0x45, 0x30, 0x32, 0x33, 0x30, 0x46, 0x32, 0x44, 0x33, 0x37, 0x32, 0x30, 0x46, 0x34, 0x30, 0x31, 0x39, 0x36, 0x39, 0x33, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x36, 0x30, 0x30, 0x30, 0x41, 0x31, 0x31, 0x44, 0x42, 0x31, 0x31, 0x44, 0x30, 0x35, 0x43, 0x30, 0x32, 0x36, 0x45, 0x38, 0x32, 0x33, 0x30, 0x46, 0x30, 0x32, 0x39, 0x36, 0x41, 0x31, 0x31, 0x44, 0x42, 0x31, 0x31, 0x44, 0x44, 0x39, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x37, 0x30, 0x30, 0x30, 0x32, 0x30, 0x39, 0x33, 0x30, 0x30, 0x30, 0x31, 0x38, 0x30, 0x39, 0x33, 0x30, 0x31, 0x30, 0x31, 0x39, 0x30, 0x39, 0x33, 0x30, 0x32, 0x30, 0x31, 0x41, 0x30, 0x39, 0x33, 0x30, 0x33, 0x30, 0x31, 0x35, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x38, 0x30, 0x30, 0x30, 0x42, 0x30, 0x39, 0x33, 0x30, 0x34, 0x30, 0x31, 0x38, 0x30, 0x39, 0x31, 0x30, 0x35, 0x30, 0x31, 0x39, 0x30, 0x39, 0x31, 0x30, 0x36, 0x30, 0x31, 0x41, 0x30, 0x39, 0x31, 0x30, 0x37, 0x30, 0x31, 0x41, 0x45, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x39, 0x30, 0x30, 0x30, 0x42, 0x30, 0x39, 0x31, 0x30, 0x38, 0x30, 0x31, 0x30, 0x31, 0x39, 0x36, 0x41, 0x31, 0x31, 0x44, 0x42, 0x31, 0x31, 0x44, 0x38, 0x30, 0x39, 0x33, 0x30, 0x35, 0x30, 0x31, 0x39, 0x30, 0x39, 0x33, 0x42, 0x35, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x41, 0x30, 0x30, 0x30, 0x30, 0x36, 0x30, 0x31, 0x41, 0x30, 0x39, 0x33, 0x30, 0x37, 0x30, 0x31, 0x42, 0x30, 0x39, 0x33, 0x30, 0x38, 0x30, 0x31, 0x42, 0x46, 0x39, 0x31, 0x41, 0x46, 0x39, 0x31, 0x39, 0x46, 0x39, 0x31, 0x30, 0x30, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x42, 0x30, 0x30, 0x30, 0x38, 0x46, 0x39, 0x31, 0x33, 0x46, 0x39, 0x31, 0x32, 0x46, 0x39, 0x31, 0x30, 0x46, 0x39, 0x30, 0x30, 0x46, 0x42, 0x45, 0x30, 0x46, 0x39, 0x30, 0x31, 0x46, 0x39, 0x30, 0x31, 0x38, 0x39, 0x35, 0x32, 0x37, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x43, 0x30, 0x30, 0x30, 0x37, 0x38, 0x39, 0x34, 0x38, 0x34, 0x42, 0x35, 0x38, 0x32, 0x36, 0x30, 0x38, 0x34, 0x42, 0x44, 0x38, 0x34, 0x42, 0x35, 0x38, 0x31, 0x36, 0x30, 0x38, 0x34, 0x42, 0x44, 0x38, 0x35, 0x42, 0x35, 0x33, 0x31, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x44, 0x30, 0x30, 0x30, 0x38, 0x32, 0x36, 0x30, 0x38, 0x35, 0x42, 0x44, 0x38, 0x35, 0x42, 0x35, 0x38, 0x31, 0x36, 0x30, 0x38, 0x35, 0x42, 0x44, 0x38, 0x30, 0x39, 0x31, 0x36, 0x45, 0x30, 0x30, 0x38, 0x31, 0x36, 0x30, 0x33, 0x44, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x45, 0x30, 0x30, 0x30, 0x38, 0x30, 0x39, 0x33, 0x36, 0x45, 0x30, 0x30, 0x31, 0x30, 0x39, 0x32, 0x38, 0x31, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x38, 0x31, 0x30, 0x30, 0x38, 0x32, 0x36, 0x30, 0x38, 0x30, 0x39, 0x33, 0x45, 0x33, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x32, 0x46, 0x30, 0x30, 0x30, 0x38, 0x31, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x38, 0x31, 0x30, 0x30, 0x38, 0x31, 0x36, 0x30, 0x38, 0x30, 0x39, 0x33, 0x38, 0x31, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x38, 0x30, 0x30, 0x30, 0x45, 0x35, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x30, 0x30, 0x30, 0x30, 0x38, 0x31, 0x36, 0x30, 0x38, 0x30, 0x39, 0x33, 0x38, 0x30, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x42, 0x31, 0x30, 0x30, 0x38, 0x34, 0x36, 0x30, 0x38, 0x30, 0x39, 0x33, 0x42, 0x31, 0x30, 0x30, 0x30, 0x46, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x31, 0x30, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x42, 0x30, 0x30, 0x30, 0x38, 0x31, 0x36, 0x30, 0x38, 0x30, 0x39, 0x33, 0x42, 0x30, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x37, 0x41, 0x30, 0x30, 0x38, 0x34, 0x36, 0x30, 0x30, 0x39, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x32, 0x30, 0x30, 0x30, 0x38, 0x30, 0x39, 0x33, 0x37, 0x41, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x37, 0x41, 0x30, 0x30, 0x38, 0x32, 0x36, 0x30, 0x38, 0x30, 0x39, 0x33, 0x37, 0x41, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x33, 0x35, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x33, 0x30, 0x30, 0x30, 0x37, 0x41, 0x30, 0x30, 0x38, 0x31, 0x36, 0x30, 0x38, 0x30, 0x39, 0x33, 0x37, 0x41, 0x30, 0x30, 0x38, 0x30, 0x39, 0x31, 0x37, 0x41, 0x30, 0x30, 0x38, 0x30, 0x36, 0x38, 0x38, 0x30, 0x39, 0x33, 0x34, 0x46, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x34, 0x30, 0x30, 0x30, 0x37, 0x41, 0x30, 0x30, 0x31, 0x30, 0x39, 0x32, 0x43, 0x31, 0x30, 0x30, 0x45, 0x44, 0x45, 0x39, 0x46, 0x30, 0x45, 0x30, 0x32, 0x34, 0x39, 0x31, 0x45, 0x39, 0x45, 0x38, 0x46, 0x30, 0x45, 0x30, 0x44, 0x34, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x35, 0x30, 0x30, 0x30, 0x38, 0x34, 0x39, 0x31, 0x38, 0x38, 0x32, 0x33, 0x39, 0x39, 0x46, 0x30, 0x39, 0x30, 0x45, 0x30, 0x38, 0x38, 0x30, 0x46, 0x39, 0x39, 0x31, 0x46, 0x46, 0x43, 0x30, 0x31, 0x45, 0x38, 0x35, 0x39, 0x35, 0x37, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x36, 0x30, 0x30, 0x30, 0x46, 0x46, 0x34, 0x46, 0x41, 0x35, 0x39, 0x31, 0x42, 0x34, 0x39, 0x31, 0x46, 0x43, 0x30, 0x31, 0x45, 0x45, 0x35, 0x38, 0x46, 0x46, 0x34, 0x46, 0x38, 0x35, 0x39, 0x31, 0x39, 0x34, 0x39, 0x31, 0x46, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x37, 0x30, 0x30, 0x30, 0x38, 0x46, 0x42, 0x37, 0x46, 0x38, 0x39, 0x34, 0x45, 0x43, 0x39, 0x31, 0x45, 0x32, 0x32, 0x42, 0x45, 0x43, 0x39, 0x33, 0x38, 0x46, 0x42, 0x46, 0x43, 0x30, 0x45, 0x30, 0x44, 0x30, 0x45, 0x30, 0x30, 0x34, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x38, 0x30, 0x30, 0x30, 0x38, 0x31, 0x45, 0x30, 0x30, 0x45, 0x39, 0x34, 0x37, 0x30, 0x30, 0x30, 0x30, 0x45, 0x39, 0x34, 0x44, 0x45, 0x30, 0x30, 0x38, 0x30, 0x45, 0x30, 0x30, 0x45, 0x39, 0x34, 0x37, 0x30, 0x30, 0x30, 0x30, 0x38, 0xd, 0xa, 0x3a, 0x31, 0x30, 0x30, 0x33, 0x39, 0x30, 0x30, 0x30, 0x30, 0x45, 0x39, 0x34, 0x44, 0x45, 0x30, 0x30, 0x32, 0x30, 0x39, 0x37, 0x41, 0x31, 0x46, 0x33, 0x30, 0x45, 0x39, 0x34, 0x30, 0x30, 0x30, 0x30, 0x46, 0x31, 0x43, 0x46, 0x46, 0x38, 0x39, 0x34, 0x41, 0x34, 0xd, 0xa, 0x3a, 0x30, 0x32, 0x30, 0x33, 0x41, 0x30, 0x30, 0x30, 0x46, 0x46, 0x43, 0x46, 0x38, 0x44, 0xd, 0xa, 0x3a, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x46, 0x46, 0xd, 0xa}; 
#define binLength 2674


	void setup()
	{
	  pinMode(3, OUTPUT);
	  Wire.begin();                        // join I2C bus (address optional for master)
	  Serial.begin(115200);
	  for(int j=0; j<5; j++){
	  	digitalWrite(3, HIGH);
	  	delay(j*100);
	  	digitalWrite(3, LOW);
	  	delay(j*j*50);
	  }
	  
	  for (int i=0; i<binLength; i++){
	   digitalWrite(3, i%2);
	   byte dataByte = pgm_read_byte_near(binData + i);
	   i2c_eeprom_write_byte(EEPROM_ADDR, i, dataByte);
	   Serial.println();
	   Serial.print("Wrote: "); Serial.print(dataByte, HEX); Serial.print(" @ ");Serial.print(i);
	   delay(100);
	  }
	  
	  for (int i=0; i<binLength; i++){            // loop for first 20 slots
	    Serial.print(i); Serial.print(":");
	    Serial.print((i2c_eeprom_read_byte(EEPROM_ADDR, i)), HEX);
//	    delay(50);
	    Serial.println();
	  }  
	    pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

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


	
