#pragma once
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "log.h"
#include "network/KlipperApi.h"
#include "soc/rtc_wdt.h"

class Watchdog {
public:
  Watchdog(KlipperApi *api) {
    this->api = api;
    xTaskCreatePinnedToCore(
        refreshSceneCallback, "checkWorking", 16000, /* Stack size in words */
        this,
        21,       // see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/speed.html
        NULL, 0); /* Core ID */
  }

private:
  KlipperApi *api;

  void tick() {
    ulong lastTick = this->api->getLastSuccessfullCall();
    ulong now = millis();
    // if more than 15 mins since last connect - reboot esp
    if (now - lastTick > 900000) {
      LV_LOG_WARN("No data from Klipper for 15 mins. Rebooting...");
      ESP.restart();
    }
  }

  static void refreshSceneCallback(void *arg) {
    esp_task_wdt_add(NULL);
    while (true) {
      esp_task_wdt_reset();
      ((Watchdog *)arg)->tick();
      vTaskDelay(15);
    }
  }
};
