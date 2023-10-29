#include "DisplayHAL.h"
#include "SceneManager.h"
#include "button/Button.h"
#include "config/Config.h"
#include "fs/lv_port_fs_littlefs.h"
#include "network/KlipperApi.h"
#include "network/KnomiWebServer.h"
#include "network/WifiManager.h"
#include "scenes/Styles.h"
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
Styles *styles = nullptr;

uint32_t scenerefresh_nexttime = 0;
uint32_t keyscan_nexttime = 0;
uint32_t netcheck_nexttime = 0;
uint32_t klipper_nexttime = 0;

#if LV_USE_LOG
void logToSerial(const char *logLine) {
  String detailedLog = String(logLine);
  detailedLog.remove(detailedLog.length() - 1);
  detailedLog += String("(free heap = ") + esp_get_free_heap_size() + ")\n";
  Serial.print(detailedLog);
  if (webServer != nullptr) {
    webServer->websocketLog(detailedLog.c_str());
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
  LV_LOG_INFO("Config created");
  wifiManager = new WifiManager(config);
  LV_LOG_INFO("WifiManager created");
  btn = new Button(config);
  LV_LOG_INFO("Timer and button created");
  displayhal = new DisplayHAL();
  LV_LOG_INFO("DisplayHAL created");
  lv_port_littlefs_init();
  LV_LOG_INFO("LVFS-Littlefs proxy enabled");
  klipperApi = new KlipperApi(config);
  LV_LOG_INFO("KlipperAPI started");
  webServer = new KnomiWebServer(config, wifiManager);
  styles = new Styles(config->getUiConfig());
  LV_LOG_INFO("WebServer started");
  sceneManager = new SceneManager(webServer, klipperApi, wifiManager, styles);
  lv_timer_handler_run_in_period(33); // 30fps
  LV_LOG_INFO("SceneManager started");
  wifiManager->connectToWiFi();
  LV_LOG_INFO("Connected to wifi");
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

  if (nowtime > scenerefresh_nexttime) {
    sceneManager->RefreshScene();
    scenerefresh_nexttime = nowtime + 50;
  }

  if (nowtime > keyscan_nexttime) {
    sceneManager->Timer();
    keyscan_nexttime = nowtime + 400;
  }

  if (nowtime > klipper_nexttime && WiFi.isConnected()) {
    klipperApi->refreshData();
    klipper_nexttime = nowtime + 2000;
  }

  if (nowtime > netcheck_nexttime) {
    wifiManager->tick();
    webServer->tick();
    netcheck_nexttime = nowtime + 100UL;
  }
}
