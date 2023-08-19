#pragma once

#include <EEPROM.h>

class WifiConfig {
private:
  struct config_type {
    char stassid[32];    // 定义配网得到的WIFI名长度(最大32字节)
    char stapsw[64];     // 定义配网得到的WIFI密码长度(最大64字节)
    char klipperip[32];  // 定义配网得到的klipperip长度(最大32字节)
    char apmodeflag[32]; // 开机进入ap模式标志位(最大32字节)
  };

  config_type wificonf = {{""}, {""}, {""}, {""}};

  // EEPROM参数存储地址位
  int wifi_addr = 1; // 被写入数据的EEPROM地址编号  wifi-ssid-psw klipper

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

  void setAccessPointMode(char i) { wificonf.apmodeflag[0] = i; }
  char GetApModeFlag() { return wificonf.apmodeflag[0]; }
};
