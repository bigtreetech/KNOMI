#pragma once
#include "Arduino.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WebServer.h"
#include "Update.h"
#include "../generated/elegantWebpage.h"

class ElegantOtaClass{
  public:
    ElegantOtaClass();
    
    void setID(const char* id);
    void onOTAStart(void callable(void));
    void onOTAProgress(void callable(void));
    void onOTAEnd(void callable(void));
    void begin(WebServer *server, const char * username = "", const char * password = "");

  private:
    WebServer *_server;

    char _username[64];
    char _password[64];
    char _id[128];
    bool authenticate;      
    bool _preUpdateRequired = false;
    bool _progressUpdateRequired = false;
    bool _postUpdateRequired = false;
    void (*preUpdateCallback)();
    void (*progressUpdateCallback)();
    void (*postUpdateCallback)();
};

extern ElegantOtaClass ElegantOTA;