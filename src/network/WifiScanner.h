#pragma once
#include <esp_wifi.h>
#include <string>
#include <vector>

struct NetworkInfo {
  String networkName;
  bool isPublic;
  int signal;
};

class WifiScanner {
private:
  bool scanInProgress = false;
  unsigned long lastScan = 0;
  std::vector<NetworkInfo> foundNetworks;
  const int intervalForRescan = 5; // every 5 seconds;

public:
  WifiScanner() = default;

  std::vector<NetworkInfo> scan() {
    if (scanInProgress) {
      int foundNetworksCount = WiFi.scanComplete();
      if (foundNetworksCount >= 0) {
        scanInProgress = false;
        foundNetworks.clear();
        for (int i = 0; i < foundNetworksCount; i++) {
          String networkName = WiFi.SSID(i);
          int signal = WiFi.RSSI(i);
          bool isPublic = WiFi.encryptionType(i) == WIFI_AUTH_OPEN;

          foundNetworks.push_back({networkName, isPublic, signal});
        }
        WiFi.scanDelete();
      }
      return foundNetworks;
    }

    unsigned long now = millis();
    if (lastScan - now > intervalForRescan * 1000) {
      lastScan = now;
      WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
      if (WiFi.scanNetworks(true, true, false, 150) == WIFI_SCAN_RUNNING) {
        scanInProgress = true;
      }
    }

    return foundNetworks;
  }
};