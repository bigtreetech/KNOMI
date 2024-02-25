#pragma once
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "log.h"
#include "network/WifiManager.h"
#include "scenes/AbstractScene.h"
#include "scenes/BootupLogo.h"
#include "scenes/SwitchSceneRequest.h"
#include "soc/rtc_wdt.h"

class SceneManager {
private:
  AbstractScene *currentScene = nullptr;
  SceneId currentSceneId;
  SwitchSceneRequest *switchSceneRequest = nullptr;
  SceneDeps deps;
  Button *button;

  static void refreshSceneCallback(void *arg) {
    esp_task_wdt_add(NULL);
    while (true) {
      esp_task_wdt_reset();
      ((SceneManager *)arg)->refreshScene();
      vTaskDelay(15);
    }
  }

public:
  explicit SceneManager(KnomiWebServer *webServer, UpdateProgress *progress, KlipperApi *klipperApi,
                        WifiManager *manager, UIConfig *config, DisplayHAL *displayHAL, Button *btn)
      : deps(klipperApi, progress, manager, webServer, config, displayHAL) {
    this->currentScene = new BootupLogoScene(deps);
    this->currentSceneId = SceneId::BootupLogo;
    this->button = btn;

    xTaskCreatePinnedToCore(
        refreshSceneCallback, "displayUpdate", 16000, /* Stack size in words */
        this,
        21,       // see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/speed.html
        NULL, 0); /* Core ID */
  }

  SceneId getCurrentSceneId() { return currentSceneId; }

  void switchSceneIfRequired() {
    SwitchSceneRequest *pRequest = switchSceneRequest;
    if (pRequest != nullptr) {
      LV_LOG_INFO("Deleting current scene");
      switchSceneRequest = nullptr;

      delete currentScene;
      currentScene = nullptr;
      LV_LOG_INFO((String("Switching scene to ") + String(pRequest->id)).c_str());
      currentScene = pRequest->Provide();
      currentSceneId = pRequest->id;
      delete pRequest;
    }
  }

  void refreshScene() {
    if (deps.progress->isInProgress && this->getCurrentSceneId() != SceneId::FirmwareUpdate) {
      switchSceneRequest = new SwitchSceneRequest(deps, SceneId::FirmwareUpdate);
    } else if (deps.mgr->isInConfigMode() && this->getCurrentSceneId() != SceneId::APConfig) {
      switchSceneRequest = new SwitchSceneRequest(deps, SceneId::APConfig);
    } else if (WiFi.isConnected() && !button->isPressed()) {
      if (deps.klipperApi->isKlipperNotAvailable() && this->getCurrentSceneId() != SceneId::NoKlipper) {
        switchSceneRequest = new SwitchSceneRequest(deps, SceneId::NoKlipper);
      }
    }

    if (switchSceneRequest == nullptr) {
      switchSceneRequest = currentScene->NextScene();
    }

    if (currentScene != nullptr) {
      currentScene->Tick();
      switchSceneIfRequired();
    }
  }
};
