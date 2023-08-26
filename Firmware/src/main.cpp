#include "DisplayHAL.h"
#include "SceneManager.h"
#include "button/Button.h"
#include "fs/lv_port_fs_littlefs.h"
#include "network/KlipperApi.h"
#include "network/KnomiWebServer.h"
#include "network/WifiConfig.h"
#include "network/WifiManager.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <WiFi.h>
#include <iostream>
#include <lvgl.h>

using namespace std;

WifiConfig *wifiEepromConfig = nullptr;
WifiManager *wifiManager = nullptr;
Button *btn = nullptr;
KnomiWebServer *webServer = nullptr;
KlipperApi *klipperApi = nullptr;
SceneManager *sceneManager = nullptr;
DisplayHAL *displayhal = nullptr;
Ticker *timer = nullptr;

uint32_t keyscan_nexttime = 0;
uint32_t netcheck_nexttime = 0;

#if LV_USE_LOG
void logToSerial(const char *logLine) { Serial.print(logLine); }
#endif

__attribute__((unused)) void setup() {
  Serial.begin(115200); // 波特率
  EEPROM.begin(1024);   // 分配flash空间存储配网信息
#if LV_USE_LOG
  lv_log_register_print_cb(&logToSerial);
#endif
  delay(100);

  wifiEepromConfig = new WifiConfig();
  wifiEepromConfig->ReadConfig();
  wifiManager = new WifiManager(wifiEepromConfig);
  timer = new Ticker();
  btn = new Button(wifiManager, timer);
  displayhal = new DisplayHAL(timer);
  lv_port_littlefs_init();

  webServer = new KnomiWebServer(wifiEepromConfig, wifiManager);
  klipperApi = new KlipperApi(wifiEepromConfig);
  sceneManager = new SceneManager(klipperApi, wifiManager);
}

__attribute__((unused)) void loop() {
  lv_task_handler(); // TODO move to interrupt?

  if (WiFiClass::status() == WL_CONNECTED && !btn->isPressed()) {
    klipperApi->tick();
    if (klipperApi->isKlipperNotAvailable()) {
      sceneManager->SwitchScene(SceneId::NoKlipper, 0);
    }
  }

  sceneManager->SwitchSceneIfRequired();

  uint32_t nowtime = millis();
  if (nowtime > keyscan_nexttime) {

    sceneManager->Timer();
    klipperApi->refreshData();
    keyscan_nexttime = nowtime + 400;
  }

  if (nowtime > netcheck_nexttime) {
    wifiManager->tick();
    webServer->tick();
    netcheck_nexttime = nowtime + 100UL;
  }
}
