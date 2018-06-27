#include <Wire.h>
#include <EEPROM.h>
#include <HashMap.h>
#include "Low_RAM_NeoPixels.h"
#include "Spectrum.h"
#include "Visualizer.h"

#define EEPROM_MODE_ADDR        0
#define EEPROM_RGB_R_ADDR       1
#define EEPROM_RGB_G_ADDR       2
#define EEPROM_RGB_B_ADDR       3
#define EEPROM_CYCLE_SPEED_ADDR 4
#define EEPROM_CYCLE_NUM_ADDR   5
#define EEPROM_SPACING_ADDR     6

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// typedef void (*modePointer)(void);
using modePointer = void(*)(void);

//const byte hues[BANDS] = { 2, 21, 45, 115, 150, 200, 225 };   // Pretty
//const byte hues[BANDS] = { 2, 21, 35, 185, 200, 22a0, 240 };   // Sunset
const uint8_t hues[BANDS] = { 0, 37, 74, 111, 148, 185, 222 };   // Even

char modeLabel = 'm';
volatile uint8_t RGB[3] = {255, 32, 0};

modePointer mode;
modePointer modes[] = {
  []() { theaterChase(RGB[0], RGB[1], RGB[2]); },
  []() { showColor(RGB[0], RGB[1], RGB[2]); delay(1);},
//  []() { rainbowCycle(5 , 3); },
  Chroma
};
char modeLabels[] = {
  , 't' //heaterChase
  , 'f' //ill color
  //  ,'r'//ainbow
  , 'm' //usic visualizer
};

const PROGMEM byte HASH_SIZE = ARRAY_SIZE(modes);
HashType<char, modePointer> hashRawArray[HASH_SIZE] = {};
HashMap<char, modePointer> modeMap = HashMap<char, modePointer>(hashRawArray, ARRAY_SIZE(hashRawArray));

void receiveEvent(byte length) {
  cli();


  //New I2C protocol:
  //   [  0,  char<mode>, ... ]
  //   [init, mode,       args ]
  //NOT < 3

//  if (length < 3) {
//    modeChange();
//    ((void (*)())NULL)();
//    return;
//  }


//  for (i = 0; i < 3; i++) EEPROM.write(EEPROM_RGB_ADDR + i, RGB[i]);

  byte i = 0;
  byte payload[length];

  while (Wire.available()) { // loop through all but the last
    payload[i] = Wire.read(); // receive byte as a character
    i++;
  }

  char operation = payload[0];

  switch(operation) {
    case 'c': // change global color
      RGB[0]=payload[1]; RGB[1]=payload[2]; RGB[2]=payload[3];
      EEPROM.write(EEPROM_RGB_R_ADDR, RGB[0]);
      EEPROM.write(EEPROM_RGB_G_ADDR, RGB[1]);
      EEPROM.write(EEPROM_RGB_B_ADDR, RGB[2]);
      break;
      
    case 'm': // change active mode to specified label
      EEPROM.write(EEPROM_MODE_ADDR, payload[1]);
      default;
      
    default:
      ((void (*)())NULL)();
      return;
  }
  
  Wire.flush();
  sei();
}


void setup() {
  Serial.begin(115200);

  ledsetup();
  showColor(0, 0, 0);   //blank the display

  FFT.begin();  // Init Audio-Spectrum shield

  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event

  // Load last specified global color
  RGB[0] = EEPROM.read(EEPROM_RGB_R_ADDR);
  RGB[1] = EEPROM.read(EEPROM_RGB_G_ADDR);
  RGB[2] = EEPROM.read(EEPROM_RGB_B_ADDR);
  
  // Instantiate label -> function map to load display mode from label
  for (byte m = 0; m < HASH_SIZE; m++) modeMap[m](modeLabels[m], modes[m]);

  mode = modeMap.getValueOf(EEPROM.read(EEPROM_MODE_ADDR)); // Load display mode
}

void loop() {
  mode();
}






