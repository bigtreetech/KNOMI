#include "DisplayHAL.h"
#include "SceneManager.h"
#include "button/Button.h"
#include "config/Config.h"
#include "fs/lv_port_fs_littlefs.h"
#include "network/KlipperApi.h"
#include "network/KnomiWebServer.h"
#include "network/WifiManager.h"
#include <Arduino.h>
#include <AsyncHTTPRequest_Generic.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <iostream>
#include <lvgl.h>

using namespace std;

Config *config = nullptr;
WifiManager *wifiManager = nullptr;
Button *btn = nullptr;
KnomiWebServer *webServer = nullptr;
KlipperApi *klipperApi = nullptr;
SceneManager *sceneManager = nullptr;
__attribute__((unused)) DisplayHAL *displayhal = nullptr;

uint32_t keyscan_nexttime = 0;
uint32_t netcheck_nexttime = 0;
uint32_t klipper_nexttime = 0;

#if LV_USE_LOG
void logToSerial(const char *logLine) {
  Serial.print(logLine);
  if (webServer != nullptr) {
    webServer->websocketLog(logLine);
  }
}
#endif

__attribute__((unused)) void setup() {
  Serial.begin(115200); // 波特率
#if LV_USE_LOG
  lv_log_register_print_cb(&logToSerial);
#endif
  delay(100);

  config = new Config();
  LV_LOG_INFO(("Config created, free heap = " + String(esp_get_free_heap_size())).c_str());
  wifiManager = new WifiManager(config);
  LV_LOG_INFO(("WifiManager created, free heap = " + String(esp_get_free_heap_size())).c_str());
  btn = new Button(config);
  LV_LOG_INFO(("Timer and button created, free heap = " + String(esp_get_free_heap_size())).c_str());
  displayhal = new DisplayHAL();
  LV_LOG_INFO(("DisplayHAL created, free heap = " + String(esp_get_free_heap_size())).c_str());
  lv_port_littlefs_init();
  LV_LOG_INFO(("LVFS-Littlefs proxy enabled, free heap = " + String(esp_get_free_heap_size())).c_str());
  klipperApi = new KlipperApi(config);
  LV_LOG_INFO(("KlipperAPI started, free heap = " + String(esp_get_free_heap_size())).c_str());
  webServer = new KnomiWebServer(config, wifiManager);
  LV_LOG_INFO(("WebServer started, free heap = " + String(esp_get_free_heap_size())).c_str());
  sceneManager = new SceneManager(webServer, klipperApi, wifiManager);
  lv_timer_handler_run_in_period(33); // 30fps
  LV_LOG_INFO(("SceneManager started, free heap = " + String(esp_get_free_heap_size())).c_str());
  wifiManager->connectToWiFi();
  LV_LOG_INFO(("Connected to wifi, free heap = " + String(esp_get_free_heap_size())).c_str());
}

__attribute__((unused)) void loop() {
  lv_timer_handler_run_in_period(33); // 30fps

  if (webServer->isUpdateInProgress() && sceneManager->getCurrentSceneId() != SceneId::FirmwareUpdate) {
    sceneManager->SwitchScene(SceneId::FirmwareUpdate, 0);
  } else if (WiFi.isConnected() && !btn->isPressed()) {
    if (klipperApi->isKlipperNotAvailable() && sceneManager->getCurrentSceneId() != SceneId::NoKlipper) {
      sceneManager->SwitchScene(SceneId::NoKlipper, 0);
    }
  }

  sceneManager->SwitchSceneIfRequired();

  uint32_t nowtime = millis();
  if (nowtime > keyscan_nexttime) {
    sceneManager->Timer();
    keyscan_nexttime = nowtime + 400;
  }

  if (nowtime > klipper_nexttime) {
    klipperApi->refreshData();
    klipper_nexttime = nowtime + 2000;
  }

  if (nowtime > netcheck_nexttime) {
    wifiManager->tick();
    webServer->tick();
    netcheck_nexttime = nowtime + 100UL;
  }
}
