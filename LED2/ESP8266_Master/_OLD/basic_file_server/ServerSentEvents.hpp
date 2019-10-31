extern AsyncWebServer server;

namespace ServerSentEvents {
    AsyncEventSource source("/events");
    
    /*
        .count()
        .canHandle(req)
        .handleRequest(req)
        .send(message, event, id, reconnect)
    */
    
    void begin(void) {
        source.onConnect([](AsyncEventSourceClient *client){
            char p[32];
            sprintf(p, "Connection, %d clients\n", source.count());
            client->send(p,NULL,millis(),10000);
        });
        
        server.addHandler(&source);
    }
}