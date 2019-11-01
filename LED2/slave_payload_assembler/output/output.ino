
#include <Wire.h>
#define EEPROM_ADDR 0x50           // I2C Buss address of 24LC256 256K EEPROM
#define binLength 2054


	void setup()
	{
	  pinMode(3, OUTPUT);
	  Wire.begin();                        // join I2C bus (address optional for master)
	  Serial.begin(115200);
	  
	  for (int i=0; i<binLength; i++){
	   digitalWrite(3, i%2);
	   byte dataByte = pgm_read_byte_near(binData + i);
	   i2c_eeprom_write_byte(EEPROM_ADDR, i, dataByte);
	   Serial.println();
	   Serial.print("Wrote: "); Serial.print(dataByte, HEX); Serial.print(" @ ");Serial.print(i);
	   delay(5);
	  }
	  
	bool failure=false;
	  for (int i=0; i<binLength; i++){            // loop for first 20 slots
	    
      byte source=pgm_read_byte_near(binData + i);
      byte written=i2c_eeprom_read_byte(EEPROM_ADDR, i);
	    if (source != written) {
			Serial.print("	 !!! ");
			Serial.print("(");
			Serial.print(source, HEX); 
			Serial.print(") ");
     failure=true;
		}
	    Serial.print(i); Serial.print(":");
	    Serial.print(written, HEX);
	    delay(5);
	    Serial.println();
	  }  
	   
	  if (failure) Serial.println("Something failed");	
	  else Serial.println("EEPROM WRITE SUCCESS");
	   
	  
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


	