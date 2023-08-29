#pragma once
#include "../Version.h"
#include "WifiConfig.h"
#include "WifiManager.h"
#include "lvgl.h"
#include <ESPmDNS.h>
#include <ElegantOTA.h>
#include <WebServer.h>

// 端口号,最大连接数

class KnomiWebServer {
private:
  const int webPort = 80;
  bool started;
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

  void tick()
  {
    if (!this->started && WiFiClass::status() == WL_CONNECTED) {
      this->started = true;
      this->server->begin(webPort);
      LV_LOG_INFO("WebServer started");
    }
    this->server->handleClient();
  }

};