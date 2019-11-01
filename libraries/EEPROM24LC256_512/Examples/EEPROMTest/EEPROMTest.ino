/*************************************************** 
  This is a library for the 24LC256 & 24LC512 i2c EEPROM under the
  Arduino Platform.
  
  It provides an easy way to use the above chips while supplying 
  the functionality to read and write pages (64 or 128 bytes) and 
  individual bytes to a page number or address on the EEPROM.
  
  Each page is written in a 16 byte burst mode to account for the
  buffer limit of the Wire library in the Arduino Platform.

  This library is to be used "as-is" without warranty. Please report
  bugs to the GitHub Project.
  
  Written by Michael Neiderhauser.  October 10, 2012
  LGPL license, all text above must be included in any redistribution
 ****************************************************/


#include <Wire.h>
#include <EEPROM24LC256_512.h>

unsigned int randomNum=0;

//define eeprom chip (1 per chip)
EEPROM256_512 mem_1;

//arrays act as buffers for the EEPROM Chips
byte storage256[64];

void setup() {

  //begin I2C Bus
  Wire.begin();
  
  //begin EEPROM with I2C Address 
  mem_1.begin(0,0);//addr 0 (DEC) type 0 (defined as 24LC256)
  
  
  //begin serial for test driver output
  Serial.begin(115200);
  
  //Test 24LC256 Chip
  Test256();
  
}

void Test256()
{
  //print page size
  Serial.print("24LC256 Page Size: ");
  Serial.println(mem_1.getPageSize());
  
  //exercise byte write/read
  // test256ByteSequence();
  test256PageSequence();
}

void loop() {/*Run once Application*/}
