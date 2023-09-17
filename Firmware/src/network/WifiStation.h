#pragma once
#include "../config/NetworkConfig.h"

class WifiStation {
private:
  NetworkConfig *config;

public:
  WifiStation(NetworkConfig *config) {
    this->config = config;
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);

    WiFi.begin(config->getSsid(), config->getPsk());
  }

  ~WifiStation() { WiFi.disconnect(true); }
};