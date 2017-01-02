/******************************************************************************
  Digital Handpan Code for Teesny
  Joel Bartlett @ SparkFun Electronics
  Original Creation Date: October 16, 2016

  This sketch produces polyphonic notes when any number of eight capacitive touch pads are
  touched, thus replicating a Handpan (or Hang) steel drum. This sketch
  makes use of the Teensy Audio Shield (https://www.sparkfun.com/products/12767)
  as well as the Teensy Audio System Design Tool (http://www.pjrc.com/teensy/gui/).
  All capactive touch sensing is handled by the Teensy's built in Touch Sense Pins.

  Hardware:
    Teensy 3.2 Dev Board (https://www.sparkfun.com/products/13736)
    Teensy Audio Board (https://www.sparkfun.com/products/12767)
    Various buttons and potentiometers

  Development environment specifics:
    IDE: Arduino IDE V 1.6.11 with the Teensyduino Add-on installed V Teensy Loader V1.30

  This code is beerware; if you see me (or any other SparkFun
  employee) at the local, and you've found our code helpful,
  please buy us a round!
  Distributed as-is; no warranty is given.
*******************************************************************************/
//These libraries are included by the Teensy Audio System Design Tool
#include <Audio.h>
// Wire
//Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_400);
//#include <Wire.h>
#include <i2c_t3.h>
#include <SPI.h>
//#include <SD.h>//not using the SD library at this time
#include <SerialFlash.h>
#include <Bounce.h>

#include "note_frequency.h"
#include "scales.h"
#include "TeensyAudioDesignTool.h"

//an array to define each of the eight touch sense pins,
//all other touch sense pins are used by the Teensy Audio Shield (16,17,18,19)
int pinTouch[] = {18, 32, 17, 16, 15, 1, 0, 19};
//int avg[] = {0, 0, 0, 0, 0, 0, 0, 0};
int thr[] = {2500, 2000, 5000, 3000, 3500, 5000, 2500, 2500};
int avg[] = {2500, 2000, 5000, 3000, 3500, 5000, 2500, 2500};
//int avg[] = {0};
int sd[8] = {0};

int scale_index = 0;//var to keep track 2fo which scale is being used

int dcVal = 0;//value to control the decay of each note

//buttons for incrementing or decrementing through each scale
Bounce button0 = Bounce(2, 15);
//Bounce button1 = Bounce(3, 15);

bool debug = 0;//Set to 1 for Serial debugging

////////////////////////////////////////////////////////
//AudioSynthWaveformSine sines[] = { sine1, sine2, sine3, sine4, sine5, sine6, sine7, sine8 };
//AudioSynthWaveformDc dcs[] = { dc1, dc2, dc3, dc4, dc5, dc6, dc7, dc8 };

void setup() {
  //initialize buttons
  pinMode(2, INPUT_PULLUP);
  //pinMode(3, INPUT_PULLUP);
  button0.update();
  //button1.update();

  //  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_29_30, I2C_PULLUP_INT, I2C_RATE_400);
  //initialize Serial
  Serial.begin(115200);

  //set aside audio memory
  AudioMemory(64);

  //initialize audio settings
  sine1.amplitude(0.125);
  sine1.frequency(440);
  sine2.amplitude(0.125);
  sine2.frequency(440);
  sine3.amplitude(0.125);
  sine3.frequency(440);
  sine4.amplitude(0.125);
  sine4.frequency(440);
  sine5.amplitude(0.125);
  sine5.frequency(440);
  sine6.amplitude(0.125);
  sine6.frequency(440);
  sine7.amplitude(0.125);
  sine7.frequency(440);
  sine8.amplitude(0.125);
  sine8.frequency(440);
  //
  dc1.amplitude(0);
  dc2.amplitude(0);
  dc3.amplitude(0);
  dc4.amplitude(0);
  dc5.amplitude(0);
  dc6.amplitude(0);
  dc7.amplitude(0);
  dc8.amplitude(0);


  //sines[0] = sine1; sines[1] = sine2; sines[2] = sine3; sines[3] = sine4; sines[4] = sine5; sines[5] = sine6; sines[6] = sine7; sines[7] = sine8;

  //dcs[0] = dc1; dcs[1] = dc2; dcs[2] = dc3; dcs[3] = dc4; dcs[4] = dc5; dcs[5] = dc6; dcs[6] = dc7; dcs[7] = dc8;

  //initialize volume
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.65);
  //enable line out for troubleshooting on O-scope
  //sgtl5000_1.unmuteLineout();



}
//////////////////////////////////////////////
void loop() {

  volumeCheck();//check the volume knob

  touchCheck();//check if any of the capacitive pads have been touched

  dcValCheck();//check the decay knob

  buttonCheck();//check for button presses to change the scale

}
/////////////////////////////////////////////////////
void volumeCheck() {
  //check knob value for volume
  int knob = analogRead(A7);
  float vol = (float)knob / 1280.0;
  sgtl5000_1.volume(vol);
}
/////////////////////////////////////////////////////
void dcValCheck() {
  //check knob and set value as delay on dc constant for sine wave decay
  //dcVal = map(analogRead(A6), 0, 1023, 1, 1000);
  dcVal = 600;

  if (debug == 1)
  {
    Serial.print("A6 = ");
    Serial.println(dcVal);
  }

}
/////////////////////////////////////////////////////
void touchCheck() {
  //Each capacitive touch pad will vary based on the size and material it is made of
  //The value necessary to trigger each note will require some trial and error to get the
  //sensitivity just right. Try adjusting these values to get the best response.
  for (int i = 0; i < 8; i++) {
    int red = touchRead(pinTouch[i]);
    avg[i] = (avg[i] + red) / 2;
    int s = sqrt(pow(red - avg[i], 2) / 2);
    sd[i] = (s+sd[i])/2;


    //Serial.print(pinTouch[i]);
    //Serial.print("/");
    //Serial.print(i);
    //Serial.print("-");
    //Serial.print(touchRead(pinTouch[i]));
    Serial.print(sd[i]);
    Serial.print("\t");
  }
  Serial.println("");
  delay(50);

  //2  3 4 8 12  6 2 2

  
  if (sd[0] > 20) {
    sine1.frequency(note_frequency[scale[scale_index][0]]);
    dc1.amplitude(1.0, 5);
    
  }
  else dc1.amplitude(0, dcVal);


  if (sd[1] > 20) {
    sine2.frequency(note_frequency[scale[scale_index][1]]);
    dc2.amplitude(1.0, 5);
   
  }
  else dc2.amplitude(0, dcVal);


  if (sd[2] > 20) {
    sine3.frequency(note_frequency[scale[scale_index][2]]);
    dc3.amplitude(1.0, 5);
   
  }
  else dc3.amplitude(0, dcVal);


  if (sd[3] > 20) {
    sine4.frequency(note_frequency[scale[scale_index][3]]);
    dc4.amplitude(1.0, 5);
    
  }
  else dc4.amplitude(0, dcVal);


  if (sd[4] > 20)    {
    sine5.frequency(note_frequency[scale[scale_index][4]]);
    dc5.amplitude(1.0, 5);
    
  }
  else dc5.amplitude(0, dcVal);

 if (sd[5] > 70)    {
    sine6.frequency(note_frequency[scale[scale_index][5]]);
    dc6.amplitude(1.0, 5);
    
  }
  else dc6.amplitude(0, dcVal);


  if (sd[6] > 20) {
    sine7.frequency(note_frequency[scale[scale_index][6]]);
    dc7.amplitude(1.0, 5);
    
  }
  else dc7.amplitude(0, dcVal);


  if (sd[7] > 20) {
    sine8.frequency(note_frequency[scale[scale_index][7]]);
    dc8.amplitude(1.0, 5);
    int x = 7;
    Serial.print(x);
    Serial.print(" - ");
    Serial.print(sd[x]);
  }
  else dc8.amplitude(0, dcVal);
}

//////////////////////////////////////////////////
void buttonCheck() {
  button0.update();
  //button1.update();

  //if button 0 is pressed, increment the scale being used
  if (button0.risingEdge())  {
    scale_index++;
    //check for overflow
    if (scale_index > numOfScales) //numOfScales variable found in the scales.h file
      scale_index = 0;

  }

  //if button 1 is pressed, decrement the scale being used
  //  if (button1.risingEdge())  {
  //    scale_index--;
  //    //check for negative numbers
  //    if (scale_index < 0)
  //      scale_index = numOfScales;//numOfScales variable found in the scales.h file
  //  }

  if (debug == 1)  {
    Serial.print("Scale = ");
    Serial.println(scale_index);
  }
}

//void Randomize() {
//  for (int i=0; i<8; i++) {
//    randomSeed(analogRead(0));
//    int j = random(8);
//    int k = pinTouch[j];
//    pinTouch[j] = pinTouch[7-j];
//    pinTouch[7-j] = k;
//
//    k = thr[j];
//    thr[j] = thr[7-j];
//    thr[7-j]=k;
//  }
//}

