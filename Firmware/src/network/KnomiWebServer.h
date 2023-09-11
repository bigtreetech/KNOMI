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
  AsyncWebSocket *socket = nullptr;
  WifiConfig *wificonfig = nullptr;
  WifiManager *wifimanager = nullptr;

  bool updateInProgress = false;
  ulong updateTotal = 0;
  ulong updateDone = 0;

public:
  KnomiWebServer(WifiConfig *config, WifiManager* manager);
  ~KnomiWebServer() {
    this->server->end();
    delete this->server;
    delete this->socket;
  }

  bool isUpdateInProgress() { return updateInProgress; }

  ulong getUpdateDone() { return updateDone; }
  ulong getUpdateTotal() { return updateTotal; }

  void tick()
  {
    if (!this->started) {
      this->started = true;
      this->server->begin();
    }
  }

  void websocketLog(const char *logString) {
    if (socket != nullptr && !socket->getClients().isEmpty()) {
      socket->textAll(logString);
    }
  }
};