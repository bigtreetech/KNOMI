#pragma once

#include <EEPROM.h>

class WifiConfig {
private:
  struct config_type {
    char stassid[32];
    char stapsw[64];
    char klipperip[32];
  };

  config_type wificonf = {{""}, {""}, {""}};

  int wifi_addr = 1;

public:
  void SaveConfig();
  void ReadConfig();
  void DeleteConfig();

  void setSSID(const String& ssid) { strcpy(wificonf.stassid, ssid.c_str()); }
  void setPassword(const String&password) { strcpy(wificonf.stapsw, password.c_str()); }
  void setKlipperIp(const String& klipperIp) { strcpy(wificonf.klipperip, klipperIp.c_str()); }

  String getSSID() { return {wificonf.stassid}; }
  String getPassword() { return { wificonf.stapsw }; }
  String getKlipperIp() { return { wificonf.klipperip }; }
};
