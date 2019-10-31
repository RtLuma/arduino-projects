#include "HTTPServer.hpp"
#include "AudioAnalyzer.hpp"
#include "Radio.hpp"


void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  HTTPServer::begin();
  AudioAnalyzer::begin();
  Radio::begin();
}


void loop() {

}
