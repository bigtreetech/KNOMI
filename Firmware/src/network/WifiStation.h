#pragma once
#include "WifiConfig.h"

class WifiStation {
private:
  WifiConfig *config;

public:
  WifiStation(WifiConfig* config) {
    this->config = config;
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);

    WiFi.begin(config->getSSID(), config->getPassword());
  }

  ~WifiStation() {
    WiFi.disconnect(true);
  }
};