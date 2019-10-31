#include "NeoPixel.hpp"
#include "AudioAnalyzer.hpp"
#include "Radio.hpp"

///////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  AudioAnalyzer::begin();
  NeoPixel::begin();
  Radio::begin();
}


///////////////////////////////////////////////////////////////////////////

void dicks(void) {
  // AudioAnalyzer::computeFFT();  
  // AudioAnalyzer::prepareFFTPayload();  
  auto __t = millis() >> 3;
  uint8_t _data[32];
  for (uint8_t i = 0; i < 32; i++) _data[i] = (__t+=5 - (4*i));
  
  radio.writeFast(_data, 32, true); radio.txStandBy();
  
  auto _t = millis();

  radio.openWritingPipe(unique_pipe2);
  
  // radio.write(&hue, 1, false);
  // radio.writeBlocking(&hue, 1, 10);
  // if (radio.writeBlocking(_data, 32, 10)) {
  if (!radio.write(_data, 32, false)) {
    Serial.print("Not sent: ");
    Serial.println(_data[0]);
  }
  
  radio.txStandBy();
  
  radio.openWritingPipe(multicast_pipe);
  t = _t; 
}


void loop() {
  dicks();
  
  // AudioAnalyzer::computeFFT();  
  
  // AudioAnalyzer::prepareFFTPayload();  
  
  // radio.writeFast(&fft_payload, FFT_SAMPLES, true); radio.txStandBy();
  
  // auto _t = millis();

  // for (uint8_t j = 0; j < 2; j++) {
  //   radio.openWritingPipe(unique_pipes[j]);
    
  //   auto __t = millis() >> 3;
  //   uint8_t _data[32];
  //   for (uint8_t i = 0; i < 32; i++) _data[i] = (__t+=5 - (4*j));
    
  //   // radio.write(&hue, 1, false);
  //   // radio.writeBlocking(&hue, 1, 10);
  //   // if (radio.writeBlocking(_data, 32, 10)) {
  //   if (!radio.write(_data, 32, false)) {
  //     Serial.print("Not sent: ");
  //     Serial.println(_data[0]);
  //   }
    
  //   radio.txStandBy();
  
  // }
  // radio.openWritingPipe(multicast_pipe);
  // t = _t; 
}

