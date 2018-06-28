#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <ArduinoOTA.h>
#include "Credentials.h"

#define SLAVE_ADDR 8
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//To-do try fast i2c shit with this, look into tinywire?

ESP8266WebServer server(80);

bool sendBytes(char data[], byte length) {
  Wire.beginTransmission(SLAVE_ADDR);
  for (byte i = 0; i < length; i++)  Wire.write(data[i]);
  return !Wire.endTransmission();
}


void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  //  ArduinoOTA.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  server.on("/", []() {
    //    if (!server.authenticate(www_username, www_password))
    //      return server.requestAuthentication();
    server.send(200, "text/plain", "Login OK");
  });

  server.on("/m", []() {
    if (!server.args()) {             // .../m?f HOPEFULLY!
      server.send(400, "text/plain", "Must specify mode label");
      return;
    }

    char payload[2];
    payload[0] = 'm';
    payload[1] = server.argName(0)[0];

    if (sendBytes(payload, 2)) {
      String msg = "Mode changed? Got label ";
      msg += payload[1];
      server.send(200, "text/plain", msg);
    }
    else server.send(404, "text/plain", "I2C error");
  });

  server.on("/c", []() {  // Change global color

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
      'c',//olor
      server.arg("r").toInt(),
      server.arg("g").toInt(),
      server.arg("b").toInt()
    };

    if (sendBytes(payload, ARRAY_SIZE(payload)))
      server.send(200, "text/plain",
                  "r: " + server.arg("r") + ", g: " + server.arg("g") + ", b: " + server.arg("b"));
    else server.send(404, "text/plain", "I2C error");
  });

  server.onNotFound([]() {

    String message = "404\n\n";
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
  //  ArduinoOTA.handle();
  server.handleClient();
}
