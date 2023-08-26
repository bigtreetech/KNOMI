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
  String id;
  const int webPort = 80;
  WebServer *server = nullptr;
  WifiConfig *wificonfig = nullptr;
  WifiManager *wifimanager = nullptr;

  void handleRoot();

  void handleConfigWifi();

  void handleNotFound() { handleRoot(); }

public:
  KnomiWebServer(WifiConfig *config, WifiManager* manager);
  ~KnomiWebServer() {
    this->server->stop();
    delete this->server;
  }

  void tick() { this->server->handleClient(); }
};
;
