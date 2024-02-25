#pragma once
#include "esp_wifi.h"
#include "log.h"
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>

class WifiAccessPoint {
private:
  const char *AP_SSID = "BTT-KNOMI-";
  const byte DNS_PORT = 53;
  DNSServer *dnsService = nullptr;

public:
  WifiAccessPoint() {
    unsigned int chipId = 0;
    uint64_t mac = ESP.getEfuseMac();
    for (int i = 0; i < 17; i = i + 8) {
      chipId |= ((mac >> (40 - i)) & 0xff) << i;
    }

    IPAddress apIP(192, 9, 200, 1);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    if (WiFi.softAP(String(AP_SSID) + String(chipId, HEX))) {
      vTaskDelay(100 / portTICK_PERIOD_MS); // Add a small delay
      dnsService = new DNSServer();
      dnsService->setErrorReplyCode(DNSReplyCode::NoError);
      dnsService->start(DNS_PORT, "*", apIP);

      MDNS.begin("KNOMI");
      LV_LOG_INFO("ESP-32S SoftAP is right.");
      LV_LOG_INFO("Soft-AP IP address = ");
      LV_LOG_INFO(WiFi.softAPIP().toString().c_str());
      LV_LOG_INFO((String("MAC address = ") + WiFi.softAPmacAddress().c_str()).c_str());
    } else {
      LV_LOG_INFO("WiFiAP Failed");
      delay(1000);
      LV_LOG_INFO("restart now...");
      ESP.restart();
    }
  }

  ~WifiAccessPoint() {
    if (dnsService != nullptr) {
      dnsService->stop();
      delete dnsService;
    }
    MDNS.end();
    WiFi.softAPdisconnect(true);
    delay(2000);
    WiFi.softAPdisconnect();
  }

  void tick() {
    if (dnsService != nullptr) {
      dnsService->processNextRequest();
    }
  }
};
