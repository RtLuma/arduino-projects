#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
// #include <SPIFFS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include "ServerSentEvents.hpp"
#include "Glob.hpp"
#include "Utils.h"
// #include <ArduinoJson.h>


// SKETCH BEGIN
AsyncWebServer server(80);

const char* ssid = "Bartnett";
const char* password = "90-=op[]()_+OP}{";
const char* hostName = "esp8266";
const char* http_username = "admin";
const char* http_password = "admin";


void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin(ssid, password);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }
  
  MDNS.addService("http","tcp",80);

  SPIFFS.begin();
  
  ServerSentEvents::begin();

  // SPIFFSEditor(const String& username=String(), const String& password=String(), const fs::FS& fs=SPIFFS);
  server.addHandler(new SPIFFSEditor(http_username, http_password, SPIFFS));
  
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");
  // server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(SPIFFS, "/config.json", "text/plain"); });
  // server.serveStatic("/config", SPIFFS, "/config.json");

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });
  
  server.on("/longreply", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    for (volatile int j=0; j < 100; j++) {
      response->printf("%d ", j);
      char p[32];
      sprintf(p, "Progress: %u%%\n", j);
      ServerSentEvents::source.send(p, "progress");
    }
    request->send(response);
    // request->send(200, "text/plain", response);
    
    ServerSentEvents::source.send("longreply!",NULL,millis(),1000);
    
  });
  
  // server.addRewrite( new OneParamRewrite("/fs/{path}", "/fs?path=/{path}") );
  // server.rewrite("/fs", "/fs?path=/"); // Maybe don't want this
  server.addRewrite( new GlobRewrite("/fs/*", "/fs?path=/*") );
  
  server.on("/fs", HTTP_ANY, [](AsyncWebServerRequest *req){
    String path = req->hasArg("path") ? req->arg("path") : "/" ;
    
    if (path.length() > 32) return req->send(414, "text/plain", "URI too long (>32): "+path);
    
    bool isDir = path[path.length() - 1] == '/';
    
    switch (req->method()) {
      case HTTP_GET: {
        // File file = SPIFFS.open(path, "r");
        if (isDir) {
          Dir dir   = SPIFFS.openDir(path);
          String json="[";
          while (dir.next()) {
            File f = dir.openFile("r");
            if (json != "[") json += ',';
            json += "{\"type\":\"";
            json += (f.isFile()) ? "file" : "dir";
            json += "\",\"name\":\"";
            if (f.name()[0] == '/') json += &(f.name()[1]);
            else json += f.name();
            json += "\",\"size\":\"";
            json += (dir.fileSize());
            json += "\"}";
            f.close();
          }
          json += "]";  
          req->send(200, "text/json", json);
        }
        else if (SPIFFS.exists(path)) {
          req->send(SPIFFS, path, getMIME(path));
        }
        else {
          req->send(400, "text/plain", "Resource not found: "+path);
        }
        break;
      }
      case HTTP_PUT: {
        if (isDir) {
          Dir dir   = SPIFFS.openDir(path);
          if (dir.isDirectory()) req->send(200, "text/json", "dir found");
          else req->send(404, "text/json", "no dir");
        }
        break;
      }
      case HTTP_DELETE:  Serial.printf("DELETE");
      case HTTP_POST:    Serial.printf("POST");
      case HTTP_PATCH:   Serial.printf("PATCH");
      case HTTP_HEAD:    Serial.printf("HEAD");
      case HTTP_OPTIONS: Serial.printf("OPTIONS");
      default:           Serial.printf("UNKNOWN");
      req->send(501, "text/plain", "Not yet implemented\n");
    }
    
    
    /*
    GET "/(.*)" $1 != "" ? File : Tree list dir w/ links n such maybe
    PUT "/(.+)" 
      Body data ? {
        $1 ? Upload file to path, assume it retains filename
        else -> Upload file to "/"
      }
      else -> 400
    DELETE -> done
    */
       
  });
  
//   server.on("/fs", HTTP_DELETE, [](AsyncWebServerRequest *req){
//     String path = req->arg("path");
//     if (path == "/") return req->send(400, "text/plain", "BAD PATH");
//     if (!SPIFFS.exists(path)) return req->send(404, "text/plain", "FileNotFound");
//     SPIFFS.remove(path);
//     req->send(200, "text/plain", "");
//     path = String();
//   });
  



  
  //////////////////////////////////////////////////////////////////////////////////////
  
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    
    String _filename = filename.startsWith("/") ? filename : "/"+filename;
    
    Serial.printf("Uploading: ");
    Serial.print(HttpEnumToString(request->method()));
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());
    
    if(!index){
      // Serial.printf("UploadStart: %s\n", filename.c_str());
      request->_tempFile = SPIFFS.open(_filename, "w+");
    }
    if(len) {
      // Serial.printf("%s", (const char*)data);
      request->_tempFile.write(data,len); // stream the incoming chunk to the opened file
    }
    if(final){
      // Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
      request->_tempFile.close(); // close the file handle as the upload is now done
      // request->redirect("/fs?");
    }
    
  });
  
  //////////////////////////////////////////////////////////////////////////////////////
  
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)               Serial.printf("BodyStart: %u\n", total);
                             Serial.printf("%s", (const char*)data);
    if(index + len == total) Serial.printf("BodyEnd: %u\n", total);
  });
  
  
  //////////////////////////////////////////////////////////////////////////////////////
  
  
  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    Serial.print(HttpEnumToString(request->method()));
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++) {
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile())      Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      else if(p->isPost()) Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      else                 Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }

    request->send(404);
  });
  
  //////////////////////////////////////////////////////////////////////////////////////
  
  
  server.begin();
  
}


void loop(){
}