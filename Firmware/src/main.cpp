#include "button/Button.h"
#include "config/Config.h"
#include "log.h"
#include "network/KlipperApi.h"
#include "network/KnomiWebServer.h"
#include "network/WifiManager.h"
#include "ui/DisplayHAL.h"
#include "ui/SceneManager.h"
#include <Arduino.h>
#include <AsyncHTTPRequest_Generic.h>
#include <WiFi.h>

using namespace std;

Config *config = nullptr;
WifiManager *wifiManager = nullptr;
Button *btn = nullptr;
KnomiWebServer *webServer = nullptr;
KlipperApi *klipperApi = nullptr;
SceneManager *sceneManager = nullptr;
DisplayHAL *displayHAL = nullptr;

uint32_t scenerefresh_nexttime = 0;
uint32_t keyscan_nexttime = 0;
uint32_t netcheck_nexttime = 0;
uint32_t klipper_nexttime = 0;

ulong lastLogTime;
void logToSerial(const char *logLevel, const char *file, int line, const char *func, const char *format, ...) {
  va_list args;
  va_start(args, format);
  char buf[512];
  char msg[256];
  ulong t = millis();
  vsnprintf(msg, sizeof(msg), format, args);
  snprintf(buf, sizeof(buf), "[%s] [%u%] \t(%lu.%03lu, +%lu)\t %s: %s\t(in %s line #%d)\n", logLevel,
           esp_get_free_heap_size(), t / 1000, t % 1000, t - lastLogTime, func, msg, file, line);
  lastLogTime = t;

  Serial.print(buf);
  if (webServer != nullptr) {
    webServer->websocketLog(buf);
  }

  va_end(args);
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
  webServer = new KnomiWebServer(config, wifiManager);
  LV_LOG_INFO("WebServer started");
  sceneManager = new SceneManager(webServer, klipperApi, wifiManager, config->getUiConfig(), displayHAL);
  sceneManager->refreshScene();
  LV_LOG_INFO("SceneManager started");
  wifiManager->connectToWiFi();
  LV_LOG_INFO("Connected to wifi");
}

__attribute__((unused)) void loop() {
  if (webServer->isUpdateInProgress() && sceneManager->getCurrentSceneId() != SceneId::FirmwareUpdate) {
    sceneManager->SwitchScene(SceneId::FirmwareUpdate, 0);
  } else if (WiFi.isConnected() && !btn->isPressed()) {
    if (klipperApi->isKlipperNotAvailable() && sceneManager->getCurrentSceneId() != SceneId::NoKlipper) {
      sceneManager->SwitchScene(SceneId::NoKlipper, 0);
    }
  }

  sceneManager->switchSceneIfRequired();

  uint32_t nowtime = millis();

  if (nowtime > scenerefresh_nexttime) {
    sceneManager->refreshScene();
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
