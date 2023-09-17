#pragma once
#include "BaseConfig.h"
#include <EEPROM.h>
#include <lvgl.h>

class LegacyConfig : BaseConfig {
protected:
  void load();

private:
  struct config_type {
    char stassid[32];
    char stapsw[64];
    char klipperip[32];
  };

  config_type wificonf = {{""}, {""}, {""}};
  int configAddress = 1;

public:
  LegacyConfig() {
    this->configNamespace = "legacy";
    this->load();
  }
  String getSsid() { return this->wificonf.stassid; }
  String getPsk() { return this->wificonf.stapsw; }
  String getKlipperHost() { return this->wificonf.klipperip; }
  void save();
};