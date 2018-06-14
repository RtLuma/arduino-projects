#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#define SLAVE_ADDR 8

//To-do try fast i2c shit with this, look into tinywire?

const char* PROGMEM ssid = "Tell My Wi-Fi Love Her";
const char* PROGMEM password = "MikeGriffinMike";
const char* PROGMEM www_username = "admin";
const char* PROGMEM www_password = "euphoria";

byte retries = 0;

ESP8266WebServer server(80);

bool sendBytes(char data[], byte length) {
//  byte retry = 0;
//  do {
    Wire.beginTransmission(SLAVE_ADDR);
    for (byte i = 0; i < length; i++)  Wire.write(data[i]);
//    retry++;
//    if (retry > 5) {
//      Wire.endTransmission();
//      checkI2CLock();
//      return false;
//    }
//  }
//  while (Wire.endTransmission() > 1); // Guarantee data was sent
  return !Wire.endTransmission();
}


void setup() {
  Wire.begin(); // join i2c bus (address optional for master)

  ArduinoOTA.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  server.on("/", []() {
    //    if (!server.authenticate(www_username, www_password))
    //      return server.requestAuthentication();
    server.send(200, "text/plain", "Login OK");
  });

  server.on("/next", []() {
    char payload[] = {'0'};
    if (sendBytes(payload, 1)) server.send(200, "text/plain", "next");
    else server.send(404, "text/plain", "I2C error");
  });

  server.on("/color", []() {

    /*
      String arg(String name);        // get request argument value by name
      String arg(int i);              // get request argument value by number
      String argName(int i);          // get request argument name by number
      int args();                     // get arguments count
      bool hasArg(String name);       // check if argument exists*/

    if (!(server.hasArg("r") && server.hasArg("g") && server.hasArg("b"))) {
      server.send(400, "text/plain", "Malformed RGB format");
      return;
    }

    char payload[] = {
      server.arg("r").toInt(),
      server.arg("g").toInt(),
      server.arg("b").toInt()
    };

    if (sendBytes(payload, 3))
      server.send(200, "text/plain",
                  "r: " + server.arg("r") + ", g: " + server.arg("g") + ", b: " + server.arg("b"));
    else server.send(404, "text/plain", "I2C error");

    //    Wire.beginTransmission(8); // transmit to device #8
    //    Wire.write(server.arg("r").toInt());
    //    Wire.write(server.arg("g").toInt());
    //    Wire.write(server.arg("b").toInt());
    //    Wire.endTransmission();    // stop transmitting

    //    server.send(200, "text/plain", );

  });

  server.onNotFound([]() {

    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
  });

  server.begin();

}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
}
