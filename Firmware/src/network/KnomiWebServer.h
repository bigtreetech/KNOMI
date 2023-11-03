#pragma once
#include "../Version.h"
#include "../config/Config.h"
#include "../generated/images.h"
#include "../generated/knomiWebpage.h"
#include "Arduino.h"
#include "Update.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WifiManager.h"
#include "log.h"
#include <ESPmDNS.h>
#include "ESPAsyncWebServer.h"

class KnomiWebServer {
private:
  const int webPort = 80;
  bool started = false;
  AsyncWebServer *server = nullptr;
  AsyncWebSocket *socket = nullptr;
  Config *config = nullptr;
  WifiManager *wifimanager = nullptr;

  bool updateInProgress = false;
  ulong updateTotal = 0;
  ulong updateDone = 0;

public:
  KnomiWebServer(Config *config, WifiManager *manager);
  ~KnomiWebServer() {
    this->server->end();
    delete this->server;
    delete this->socket;
  }

  bool isUpdateInProgress() { return updateInProgress; }

  ulong getUpdateDone() { return updateDone; }
  ulong getUpdateTotal() { return updateTotal; }

  void tick() {
    if (!this->started) {
      this->started = true;
      this->server->begin();
    } else {
      socket->cleanupClients();
    }
  }

  void websocketLog(const char *logString) {
    if (socket != nullptr && !socket->getClients().isEmpty()) {
      socket->textAll(logString);
    }
  }
};