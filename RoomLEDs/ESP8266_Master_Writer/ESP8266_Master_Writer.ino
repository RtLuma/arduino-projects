#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "Credentials.h"

#define SLAVE_ADDR 8
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

ESP8266WebServer server(80);

//bool sendBytes(char data[], byte length) {
//Wire.beginTransmission(SLAVE_ADDR);
//for (byte i = 0; i < length; i++)  Wire.write(data[i]);
//return !Wire.endTransmission();
//}


void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  ArduinoOTA.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.begin(9600);

  server.on("/", []() {
    if (!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    String message = "";
    bool succesfulQuery = true;

    if (server.args()) {
      const byte length = server.args()<<1;
      char payload[length];

      for (uint8_t i = 0; i < server.args(); i++) {
        uint8_t i2 = i << 1;
        payload[i2 + 1] =
          (payload[i2] = server.argName(i)[0]) == 'm' ?
          server.arg(i)[0] : server.arg(i).toInt();
      }
      
      Wire.beginTransmission(SLAVE_ADDR);
      for (byte i = 0; i < length; i++)  Wire.write(payload[i]);
      succesfulQuery = !Wire.endTransmission();

      for (uint8_t i = 0; i < length; i+=2) {
        char jews[] = {payload[i], '\0'};
        message += String(jews) + ": " + String((uint8_t)payload[i+1]) + "; ";
      }
    }
    else message = "No args";

    //if (succesfulQuery) server.send(200, "text/plain", "");
    if (succesfulQuery) server.send(200, "text/plain", message + "\n");
    else server.send(500, "text/plain", "I2C error?");
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
