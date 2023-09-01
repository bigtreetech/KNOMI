#pragma once
#include "../Version.h"
#include "WifiConfig.h"
#include "WifiManager.h"
#include "lvgl.h"
#include <ESPmDNS.h>
#include "Arduino.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "ESPAsyncWebServer.h"
#include "Update.h"
#include "../generated/knomiWebpage.h"

class KnomiWebServer {
private:
  const int webPort = 80;
  bool started = false;
  AsyncWebServer *server = nullptr;
  WifiConfig *wificonfig = nullptr;
  WifiManager *wifimanager = nullptr;

public:
  KnomiWebServer(WifiConfig *config, WifiManager* manager);
  ~KnomiWebServer() {
    this->server->end();
    delete this->server;
  }

  void tick()
  {
    if (!this->started) {
      this->started = true;
      this->server->begin();
    }
  }

};