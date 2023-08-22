#pragma once
#include "DnsService.h"
#include <WiFi.h>

class WifiAccessPoint {
private:
  const char *AP_SSID = "BTT-KNOMI";
  DnsService* dnsService = nullptr;

public:
  WifiAccessPoint() {
    IPAddress apIP(192, 168, 20, 1);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    if (WiFi.softAP(AP_SSID)) // 开启AP热点,如需要密码则添加第二个参数
    {
      dnsService = new DnsService(apIP);
      LV_LOG_INFO("ESP-32S SoftAP is right.");
      LV_LOG_INFO("Soft-AP IP address = ");
      LV_LOG_INFO(WiFi.softAPIP().toString().c_str());
      LV_LOG_INFO(
          (String("MAC address = ") + WiFi.softAPmacAddress().c_str()).c_str());
    } else {
      LV_LOG_INFO("WiFiAP Failed");
      delay(1000);
      LV_LOG_INFO("restart now...");
      ESP.restart();
    }
  }

  ~WifiAccessPoint() {
    delete dnsService;
  }

  void tick() {
    if (dnsService != nullptr) {
      dnsService->tick();
    }
  }
};