#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <SPIFFSEditor.h>
#include "ServerSentEvents.hpp"
#include "Glob.hpp"
#include "Utils.h"

// extern namespace Radio;
#include "Radio.hpp"


const char* ssid = "Bartnett";
const char* password = "90-=op[]()_+OP}{";
const char* hostName = "esp8266";
const char* http_username = "admin";
const char* http_password = "admin";
AsyncWebServer server(80);

namespace HTTPServer {

    void begin(void) {
    
        SPIFFS.begin();
    
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

        ServerSentEvents::begin();
        
        server.addRewrite( new GlobRewrite("/fs/*", "/fs?path=/*") );
        server.addHandler(new SPIFFSEditor(http_username, http_password, SPIFFS));
        server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");
        // server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(SPIFFS, "/config.json", "text/plain"); });
        // server.serveStatic("/config", SPIFFS, "/config.json");
        
        server.on("/ota", HTTP_GET, [](AsyncWebServerRequest *request) {
            
            #define DATA_SZ 32
            #define DATA_SZ_REAL (DATA_SZ - 1)
            
            // Make sure firmware file exists, and handle accordingly
            File f = SPIFFS.open("/fw.eeprom", "r");
            if (!f) {
                request->send(400, "text/plain", "Could not read firmware file");
                return;
            }
            
            // Widen protocol buffer so we can send more bytes at a time.
            uint8_t handshake_buffer[] = { RadioHeader::PayloadSize, DATA_SZ };
            Radio::whisper(2, handshake_buffer, 2);
            
            // Let the slave know this is important data
            handshake_buffer[0] = RadioHeader::FirmwareBegin;
            Radio::whisper(2, handshake_buffer, 1);
            
            uint8_t buffer[DATA_SZ];
            
            // Won't need to change this byte until the last page
            buffer[0] = RadioHeader::Firmware;
            
            uint16_t pages = f.size()/DATA_SZ_REAL;
            if (!f.size() % DATA_SZ_REAL) pages--;  // Bytes fit perfectly into last page.
            
            // for(int i=0; i<f.size(); i+=DATA_SZ_REAL) {
            for(int i=0; i<pages; i++) {
                ESP.wdtFeed();
                for(int j=1; j<DATA_SZ; j++) buffer[j] = f.read();
                if (!Radio::whisper(2, buffer, DATA_SZ)) {
                    f.close();
                    request->send(500, "text/plain", "Could not transmit\n");
                    return;
                }
                radio.txStandBy(); 
                
                // char p[32];
                // sprintf(p, "Sent: #%u\n", buffer[0]);
                // ServerSentEvents::source.send(p, "progress");
            }
            
            buffer[0] = RadioHeader::FirmwareEnd; // Conclude with final page
            const uint16_t remaining = f.size() - (pages*DATA_SZ_REAL) + 1; // Size of last page
            
            for(int i=1; i<remaining; i++) buffer[i] = f.read();
            
            if (!Radio::whisper(2, buffer, remaining)) {
                    f.close();
                    request->send(500, "text/plain", "Could not transmit\n");
                    return;
                }
            
            f.close(); 
            
            // Radio::shout(buffer, DATA_SZ);                
            request->send(200, "text/plain", "neat\n");
            
        });
        
        server.on("/fw", HTTP_GET, [](AsyncWebServerRequest *request) {
            File f = SPIFFS.open("/fw.eeprom", "r");
            AsyncResponseStream *response = request->beginResponseStream("text/plain");

            if (!f) response->print("file open failed");
            else {
                uint8_t  chk8  = 0;
                uint16_t chk16 = 0;
                uint32_t chk32 = 0;
                for(int i=0; i<f.size(); i++) {
                    uint8_t byt = f.read();
                    chk8  += byt;
                    chk16 += byt;
                    chk32 += byt;
                    response->print((char)byt);
                }
                
                uint16_t pages = f.size()/DATA_SZ_REAL;
                const uint16_t remaining = f.size() - (pages*DATA_SZ_REAL) + 1;
                response->printf("HECK: %d\n", remaining);
                                
                f.close();  //Close file
                response->printf("8: %d, 16:%d 32: %d\n", chk8, chk16, chk32);
            }
            request->send(response);
        });
        
        server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", String(ESP.getFreeHeap()));
        });
        
        server.on("/fs", HTTP_ANY, [](AsyncWebServerRequest *req) {
            
            String path = req->hasArg("path") ? req->arg("path") : "/" ;
            
            if (path.length() > 32)
                return req->send(414, "text/plain", "URI too long (>32): "+path);
            
            bool isDir = path[path.length() - 1] == '/';
            
            switch (req->method()) {
                case HTTP_GET: {
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
                        req->send(404, "text/plain", "Resource not found: "+path);
                    }
                    break;
                }
                case HTTP_PUT: {
                    if (isDir) {
                        if (SPIFFS.mkdir(path)) req->send(200, "text/plain", "MKDIR: "+path+"\n");
                        else req->send(400, "text/plain", "\""+path+"\" directory not made\n");
                    }
                    
                    break;
                }
                case HTTP_DELETE: {
                    if (SPIFFS.remove(path)) req->send(200, "text/plain", "DELETED: "+path+"\n");
                    else req->send(400, "text/plain", "\""+path+"\" not found.\n");
                    break;
                }
                case HTTP_POST: {
                    Serial.printf("POST");
                    req->send(100, "text/plain");
                    break;
                }
                case HTTP_PATCH: {
                    FSInfo fs_info;
                    SPIFFS.info(fs_info);
                    AsyncResponseStream *response = req->beginResponseStream("text/plain");
                    response->printf("totalBytes: %d\n", fs_info.totalBytes) ;
                    response->printf("usedBytes: %d\n", fs_info.usedBytes) ;
                    response->printf("blockSize: %d\n", fs_info.blockSize) ;
                    response->printf("pageSize: %d\n", fs_info.pageSize) ;
                    response->printf("maxOpenFiles: %d\n", fs_info.maxOpenFiles) ;
                    response->printf("maxPathLength: %d\n", fs_info.maxPathLength) ;
                    req->send(response);
                }
                case HTTP_HEAD:    Serial.printf("HEAD");
                case HTTP_OPTIONS: Serial.printf("OPTIONS");
                default:           Serial.printf("UNKNOWN");
                req->send(501, "text/plain", "Not yet implemented\n");
            }
        });
        
        server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {

            String _filename = filename.startsWith("/") ? filename : "/"+filename;
            if (request->hasParam("path")) {
                _filename = "/" + request->getParam("path")->value();
            }

            if (!index) request->_tempFile = SPIFFS.open(_filename, "w+");
            if (len) request->_tempFile.write(data,len);
            if (final) {
                request->_tempFile.close();
                request->send(200, "text/plain", "UPLOADED: "+_filename+"\n");
                // request->redirect("/fs");
            }
        });

        
        server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            if(!index)               Serial.printf("BodyStart: %u\n", total);
                                    Serial.printf("%s", (const char*)data);
            if(index + len == total) Serial.printf("BodyEnd: %u\n", total);
        });
        
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
        
        
        server.begin();
    }   
}
