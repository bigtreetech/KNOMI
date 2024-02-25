#pragma once
#include "../config/Config.h"
#include "WifiAccessPoint.h"
#include "WifiScanner.h"
#include "WifiStation.h"
#include <esp_wifi.h>

class WifiManager {
private:
  Config *config;
  NetworkConfig *networkConfig;
  WifiAccessPoint *ap = nullptr;
  WifiStation *sta = nullptr;
  WifiScanner *scanner = nullptr;

public:
  explicit WifiManager(Config *config) {
    this->config = config;
    this->networkConfig = config->getNetworkConfig();
    this->scanner = new WifiScanner();
  }

  ~WifiManager() {
    delete scanner;
    delete ap;
    delete sta;
  }

  std::vector<NetworkInfo> scan() { return this->scanner->scan(); }

  void resetWifi() {
    LV_LOG_INFO("Clearing wifi setup");
    config->reset();
    delay(500);
    WiFi.disconnect(true, true);
    esp_wifi_restore();
    LV_LOG_INFO("Saved wifi config deleted and switched mode to AP. Resetting...");
    delay(10);
    ESP.restart();
  }

  bool isInConfigMode() { return this->ap != nullptr; }

  void connectToWiFi() {
    // We are using mode wifi ap+sta. This means ESP32 can both connect to existing wifi net AND handle AP.
    // We don't want to have AP working when we have connection to wifi net established.
    // At the same time we want always to have connection to wifi (and retried, if necessary).
    // The fact that network configuration might change - it makes lives a little bit harder.
    // So the idea is:
    // - always have WifiStation working (and trying to reconnect + handling network config changes)
    // - if there is no connection from WifiStation - bring up WifiAccessPoint. Bring it down once WifiStation connects.
    WiFiClass::hostname(this->networkConfig->getHostname());
    WiFi.softAPsetHostname(this->networkConfig->getHostname().c_str());
    WiFiClass::mode(WIFI_AP_STA);

    if (sta != nullptr) {
      delete sta;
      sta = nullptr;
    }
    if (ap != nullptr) {
      delete ap;
      ap = nullptr;
    }

    if (networkConfig->getSsid().isEmpty() || !config->isInitialised()) {
      return;
    }

    sta = new WifiStation(networkConfig);
  }

  void tick() {
    if (ap != nullptr) {
      ap->tick();

      if (WiFi.isConnected()) {
        delete ap;
        ap = nullptr;
      }
    } else {
      if (!WiFi.isConnected()) {
        ap = new WifiAccessPoint();
      }
    }
  }
};