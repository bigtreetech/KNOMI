#pragma once
#include "../Version.h"
#include "WifiConfig.h"
#include "WifiManager.h"
#include "lvgl.h"
#include <ESPmDNS.h>
#include <WebServer.h>
#include "Arduino.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WebServer.h"
#include "Update.h"
#include "../generated/elegantWebpage.h"
#include "../generated/knomiWebpage.h"

class KnomiWebServer {
private:
  const int webPort = 80;
  bool started = false;
  WebServer *server = nullptr;
  WifiConfig *wificonfig = nullptr;
  WifiManager *wifimanager = nullptr;

public:
  KnomiWebServer(WifiConfig *config, WifiManager* manager);
  ~KnomiWebServer() {
    this->server->stop();
    delete this->server;
  }

  void tick()
  {
    if (!this->started) {
      this->started = true;
      this->server->begin(webPort);
    }
    this->server->handleClient();
  }

};