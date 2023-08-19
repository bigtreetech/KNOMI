#pragma once
#include "../Version.h"
#include "WifiConfig.h"
#include "lvgl.h"
#include <ESPmDNS.h>
#include <ElegantOTA.h>
#include <WebServer.h>

// 端口号,最大连接数

class KnomiWebServer {
private:
  const int webPort = 80;
  WebServer *server;
  WifiConfig *wificonfig;

  void handleRoot();

  void handleConfigWifi();

  void handleNotFound() { handleRoot(); }

public:
  KnomiWebServer(WifiConfig *config);
  ~KnomiWebServer() {
    this->server->stop();
    delete this->server;
  }

  void tick() { this->server->handleClient(); }
};
;
