#include "button/Button.h"
#include "config/Config.h"
#include "log.h"
#include "network/KlipperApi.h"
#include "network/KnomiWebServer.h"
#include "network/WifiManager.h"
#include "ui/DisplayHAL.h"
#include "ui/SceneManager.h"
#include <Arduino.h>
#include <WiFi.h>

using namespace std;

Config *config = nullptr;
WifiManager *wifiManager = nullptr;
Button *btn = nullptr;
KnomiWebServer *webServer = nullptr;
KlipperApi *klipperApi = nullptr;
__attribute__((unused)) SceneManager *sceneManager = nullptr;
DisplayHAL *displayHAL = nullptr;
UpdateProgress *progress = nullptr;

uint32_t netcheck_nexttime = 0;
uint32_t klipper_nexttime = 0;

ulong lastLogTime;
void logToSerial(const char *logLevel, const char *file, int line, const char *func, const char *format, ...) {
  va_list args;
  va_start(args, format);
  static char msg[1024];
  ulong t = millis();
  vsnprintf(msg, sizeof(msg), format, args);
  va_end(args);

  static char buf[2048];
  snprintf(buf, sizeof(buf), "[%s] \t[%u] [%s] \t(%lu.%03lu, +%lu)\t %s: %s\t(in %s:%d)\n", logLevel,
           esp_get_free_heap_size(), pcTaskGetName(xTaskGetCurrentTaskHandle()), t / 1000, t % 1000, t - lastLogTime,
           func, msg, file, line);
  lastLogTime = t;

  printf(buf);
  if (webServer != nullptr) {
    webServer->websocketLog(buf);
  }
}

__attribute__((unused)) void setup() {
  Serial.begin(115200); // 波特率
  delay(100);

  LV_LOG_INFO("Setup");
  LittleFS.begin();
  LV_LOG_INFO("LittleFS started");
  config = new Config();
  LV_LOG_INFO("Config created");
  wifiManager = new WifiManager(config);
  LV_LOG_INFO("WifiManager created");
  btn = new Button(config);
  LV_LOG_INFO("Timer and button created");
  displayHAL = new DisplayHAL();
  LV_LOG_INFO("DisplayHAL created");
  klipperApi = new KlipperApi(config);
  LV_LOG_INFO("KlipperAPI started");
  progress = new UpdateProgress();
  webServer = new KnomiWebServer(config, wifiManager, progress);
  LV_LOG_INFO("WebServer started");
  sceneManager = new SceneManager(webServer, progress, klipperApi, wifiManager, config->getUiConfig(), displayHAL, btn);
  LV_LOG_INFO("SceneManager started");
  wifiManager->connectToWiFi();
  LV_LOG_INFO("Connected to wifi");
}

__attribute__((unused)) void loop() {
  uint32_t nowtime = millis();

  if (nowtime > klipper_nexttime && WiFi.isConnected() && !progress->isInProgress) {
    klipperApi->refreshData();
    klipper_nexttime = nowtime + 2000;
  }

  if (nowtime > netcheck_nexttime) {
    wifiManager->tick();
    webServer->tick();
    netcheck_nexttime = nowtime + 100UL;
  }
}
