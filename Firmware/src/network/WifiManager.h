#pragma once
#include "WifiAccessPoint.h"
#include "WifiConfig.h"
#include "WifiStation.h"
#include <esp_wifi.h>

class WifiManager {
private:
  WifiConfig *config;
  WifiAccessPoint *ap = nullptr;
  WifiStation *sta = nullptr;

  bool _isConnected = false;

  const char *HOST_NAME = "KNOMI";

public:
  WifiManager(WifiConfig *config) {
    this->config = config;
    WiFi.hostname(HOST_NAME);
  }

  ~WifiManager() { delete ap; }

  void resetWifi() {
    config->DeleteConfig();
    delay(500);

    esp_wifi_restore();
    LV_LOG_INFO(
        "Saved wifi config deleted and switched mode to AP. Resetting...");
    delay(10);
  }

  bool isConnected() { return _isConnected; }

  bool isInConfigMode() { return this->ap != nullptr; }

  void connectToWiFi() {
    const int timeOut_s = 15;

    if (config->getSSID().isEmpty()) {
      ap = new WifiAccessPoint();
      return;
    }

    delete ap;
    ap = nullptr;
    sta = new WifiStation(config);

    while (WiFi.status() != WL_CONNECTED) {
      delay(timeOut_s * 1000);
      LV_LOG_INFO("WIFI autoconnect fail, start AP for webconfig now...");
      delete sta;
      sta = nullptr;
      this->ap = new WifiAccessPoint();
      this->_isConnected = true;
      return;
    }

    if (WiFi.status() == WL_CONNECTED) // 如果连接成功
    {
      config->SaveConfig();

      LV_LOG_INFO("WIFI connect Success");
      LV_LOG_INFO("SSID:%s", WiFi.SSID().c_str());
      LV_LOG_INFO(", PSW:%s\r\n", WiFi.psk().c_str());
      LV_LOG_INFO("LocalIP:");
      LV_LOG_INFO(WiFi.localIP().toString().c_str());
      LV_LOG_INFO(" ,GateIP:");
      LV_LOG_INFO(WiFi.gatewayIP().toString().c_str());

      LV_LOG_INFO("KlipperIP:");
      LV_LOG_INFO(config->getKlipperIp().c_str());

      LV_LOG_INFO("WIFI status is:");
      LV_LOG_INFO(String(WiFi.status()).c_str());

      this->_isConnected = true;
    }
  }

  void tick() {
    if (ap != nullptr)
      ap->tick();

    if (WiFiClass::status() != WL_CONNECTED &&
        WiFiClass::getMode() != WIFI_AP &&
        WiFiClass::getMode() != WIFI_AP_STA)
    {
      LV_LOG_INFO("WiFi Mode:");
      LV_LOG_INFO(String(WiFi.getMode()).c_str());
      connectToWiFi();
    }
  }
};