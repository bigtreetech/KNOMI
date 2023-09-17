#include "LegacyConfig.h"

void LegacyConfig::load() {
  LV_LOG_INFO("Loading LegacyConfig");
  EEPROM.begin(1024); // 分配flash空间存储配网信息

  uint8_t *p = (uint8_t *)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++) {
    *(p + i) = EEPROM.read(i + configAddress);
  }

  LV_LOG_INFO(("SSID:%s", this->getSsid().c_str()));
}

void LegacyConfig::save() { LV_LOG_INFO("LegacyConfig does not support saving."); }