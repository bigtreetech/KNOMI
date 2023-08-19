#include "WifiConfig.h"

// wifi ssid，psw保存到eeprom
void WifiConfig::SaveConfig() {
  // 开始写入
  uint8_t *p = (uint8_t *)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++) {
    EEPROM.write(i + wifi_addr, *(p + i)); // 在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit(); // 执行写入ROM
  delay(10);
}

// 删除原有eeprom中的信息
void WifiConfig::DeleteConfig() {
  config_type deletewifi = {
      {""}, {""}, {""}, {"5"}}; // 5 AP模式标志位 ， 8 STA模式
  uint8_t *p = (uint8_t *)(&deletewifi);
  for (int i = 0; i < sizeof(deletewifi); i++) {
    EEPROM.write(i + wifi_addr, *(p + i)); // 在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit(); // 执行写入ROM
  delay(10);
}

// 从eeprom读取WiFi信息ssid，psw
void WifiConfig::ReadConfig() {
  uint8_t *p = (uint8_t *)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++) {
    *(p + i) = EEPROM.read(i + wifi_addr);
  }
}