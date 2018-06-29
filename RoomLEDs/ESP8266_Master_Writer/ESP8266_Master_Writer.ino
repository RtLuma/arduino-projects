#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "Credentials.h"

#define SLAVE_ADDR 8
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

ESP8266WebServer server(80);

bool sendBytes(char data[], byte length) {
  Wire.beginTransmission(SLAVE_ADDR);
  for (byte i = 0; i < length; i++)  Wire.write(data[i]);
  return !Wire.endTransmission();
}


void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  ArduinoOTA.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.begin(9600);

  //TODO: This should be the only route u nignog ////////////////////////////////

  server.on("/", []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    String message = "";
    bool succesfulQuery = true;

    if (server.args()) {
      byte length = server.args() << 1;

      char payload[length];

      for (uint8_t i = 0; i < length; i += 2) {
        payload[i] = server.argName(i)[0];

        if (payload[i]=='m') payload[i + 1] = server.arg(i)[0];
        else payload[i + 1] = server.arg(i).toInt();
      }

      succesfulQuery = sendBytes(payload, length);

      for (uint8_t i = 0; i < length; i += 2) {
        message += payload[i];
        message += ": ";
        message += server.arg(i);
        message += " ";
      }
    }
    else message = "No args";

    if (succesfulQuery) server.send(200, "text/plain", "dicks");
    else server.send(404, "text/plain", "I2C error?");
  });
  
  ////////////////////////////////////////////////////////////////////


//  server.on("/m", []() {
//    if (!server.authenticate(www_username, www_password))
//      return server.requestAuthentication();
//
//    if (!server.args()) {             // .../m?f HOPEFULLY!
//      server.send(400, "text/plain", "Must specify mode label");
//      return;
//    }
//
//    char payload[2];
//    payload[0] = 'm';
//    payload[1] = server.argName(0)[0];
//
//    if (sendBytes(payload, 2)) {
//      String msg = "Mode changed? Got label ";
//      msg += payload[1];
//      server.send(200, "text/plain", msg);
//    }
//    else server.send(404, "text/plain", "I2C error");
//  });

//  server.on("/c", []() {  // Change global color
//    if (!server.authenticate(www_username, www_password))
//      return server.requestAuthentication();
//
//    /*
//      String arg(String name);        // get request argument value by name
//      String arg(int i);              // get request argument value by number
//      String argName(int i);          // get request argument name by number
//      int args();                     // get arguments count
//      bool hasArg(String name);       // check if argument exists*/
//
//    if (!(server.hasArg("r") && server.hasArg("g") && server.hasArg("b"))) {
//      server.send(400, "text/plain", "Malformed RGB format");
//      return;
//    }
//
//    char payload[] = {
//      'c',//olor
//      server.arg("r").toInt(),
//      server.arg("g").toInt(),
//      server.arg("b").toInt()
//    };
//
//    if (sendBytes(payload, ARRAY_SIZE(payload)))
//      server.send(200, "text/plain",
//                  "r: " + server.arg("r") + ", g: " + server.arg("g") + ", b: " + server.arg("b"));
//    else server.send(404, "text/plain", "I2C error");
//  });

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
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}
