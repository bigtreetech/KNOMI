#pragma once
#include "../config/Config.h"
#include "WifiAccessPoint.h"
#include "WifiStation.h"
#include <esp_wifi.h>

class WifiManager {
private:
  Config *config;
  NetworkConfig *networkConfig;
  WifiAccessPoint *ap = nullptr;
  WifiStation *sta = nullptr;

  bool _isConnected = false;

public:
  explicit WifiManager(Config *config) {
    this->config = config;
    this->networkConfig = config->getNetworkConfig();
    WiFi.mode(WIFI_AP_STA);
  }

  ~WifiManager() { delete ap; }

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

  bool isConnected() const { return _isConnected; }

  bool isInConfigMode() { return this->ap != nullptr; }

  void connectToWiFi() {
    int timeOut_s = 30;
    WiFiClass::hostname(this->networkConfig->getHostname().c_str());

    if (networkConfig->getSsid().isEmpty() || !config->isInitialised()) {
      LV_LOG_INFO("Config is not initailised starting AP mode");
      ap = new WifiAccessPoint();
      return;
    }
    LV_LOG_INFO("We have config - let's try STA");

    delete ap;
    ap = nullptr;

    sta = new WifiStation(networkConfig);

    while (WiFiClass::status() != WL_CONNECTED && timeOut_s > 0) {
      timeOut_s -= 1;
      delay(1000);
    }

    if (WiFiClass::status() != WL_CONNECTED) {
      LV_LOG_INFO("WIFI autoconnect fail, start AP for webconfig now...");
      delete sta;
      sta = nullptr;
      this->ap = new WifiAccessPoint();
      return;
    }

    LV_LOG_INFO("STA connected");

    if (WiFiClass::status() == WL_CONNECTED) // 如果连接成功
    {
      LV_LOG_INFO("WIFI connect Success");
      LV_LOG_INFO("SSID:%s", WiFi.SSID().c_str());
      LV_LOG_INFO(", PSW:%s\r\n", WiFi.psk().c_str());
      LV_LOG_INFO("LocalIP:");
      LV_LOG_INFO(WiFi.localIP().toString().c_str());
      LV_LOG_INFO(" ,GateIP:");
      LV_LOG_INFO(WiFi.gatewayIP().toString().c_str());

      LV_LOG_INFO("WIFI status is:");
      LV_LOG_INFO(String(WiFi.status()).c_str());

      this->_isConnected = true;
    }
  }

  void tick() {
    if (ap != nullptr)
      ap->tick();

    if (WiFiClass::status() != WL_CONNECTED && WiFiClass::getMode() != WIFI_AP && WiFiClass::getMode() != WIFI_AP_STA) {
      LV_LOG_INFO("WiFi Mode:");
      LV_LOG_INFO(String(WiFi.getMode()).c_str());
      connectToWiFi();
    }
  }
};