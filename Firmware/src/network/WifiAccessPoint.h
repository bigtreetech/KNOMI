#pragma once
#include "esp_wifi.h"
#include "lvgl.h"
#include <DNSServer.h>
#include <WiFi.h>

class WifiAccessPoint {
private:
  const char *AP_SSID = "BTT-KNOMI";
  const byte DNS_PORT = 53; // 设置DNS端口号
  DNSServer *dnsService = nullptr;

public:
  WifiAccessPoint() {
    IPAddress apIP(192, 168, 1, 1);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    if (WiFi.softAP(AP_SSID)) // 开启AP热点,如需要密码则添加第二个参数
    {
      // Disable AMPDU RX on the ESP32 WiFi to fix a bug on Android
      esp_wifi_stop();
      esp_wifi_deinit();
      wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
      my_config.ampdu_rx_enable = false;
      esp_wifi_init(&my_config);
      esp_wifi_start();
      vTaskDelay(100 / portTICK_PERIOD_MS); // Add a small delay

      dnsService = new DNSServer();
      dnsService->start(DNS_PORT, "*", apIP);
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