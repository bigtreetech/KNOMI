#pragma once
#include "log.h"
#include "network/WifiManager.h"
#include "scenes/AbstractScene.h"
#include "scenes/BootupLogo.h"
#include "scenes/SwitchSceneRequest.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "soc/rtc_wdt.h"

class SceneManager {
private:
  uint8_t timer_contne = 0;
  AbstractScene *currentScene = nullptr;
  SceneId currentSceneId;
  SwitchSceneRequest *switchSceneRequest = nullptr;
  SceneDeps deps;

  static void refreshSceneCallback(void* arg) {
    esp_task_wdt_add(NULL);
    while (true) {
      esp_task_wdt_reset();
      ((SceneManager*)arg)->refreshScene();
    }
  }

public:
  explicit SceneManager(KnomiWebServer *webServer, KlipperApi *klipperApi, WifiManager *manager, UIConfig *config,
                        DisplayHAL *displayHAL)
      : deps(klipperApi, manager, webServer, config, displayHAL) {
    this->currentScene = new BootupLogoScene(deps);
    this->currentSceneId = SceneId::BootupLogo;

    xTaskCreatePinnedToCore(
        refreshSceneCallback,
        "displayUpdateTask",
        10000,      /* Stack size in words */
        this,
        0,
        NULL,
        0);         /* Core ID */
  }

  ~SceneManager() {
    //esp_timer_stop(periodic_timer);
    //esp_timer_delete(periodic_timer);
  }

  SceneId getCurrentSceneId() { return currentSceneId; }

  void switchSceneIfRequired() {
    if (switchSceneRequest != nullptr) {
      LV_LOG_INFO("Deleting current scene");
      delete currentScene;
      currentScene = nullptr;
      LV_LOG_INFO((String("Switching scene to ") + String(switchSceneRequest->id)).c_str());
      currentScene = switchSceneRequest->Provide();
      currentSceneId = switchSceneRequest->id;
      delete switchSceneRequest;
      switchSceneRequest = nullptr;
    }
  }

  void refreshScene() {
    if (currentScene != nullptr) {
      currentScene->Tick();
      switchSceneIfRequired();
    }
  }

  void Timer() {
    if (timer_contne > 0)
      timer_contne--;

    if (timer_contne == 0) {
      if (currentScene != nullptr) {
        // todo reimplement this timer_contne to scene actually being able to track it's lifetime in millis + merge tick
        // and nextscene();
        timer_contne = 5;
        switchSceneRequest = currentScene->NextScene();
        if (switchSceneRequest != nullptr && switchSceneRequest->timerOverride >= 0) {
          timer_contne = switchSceneRequest->timerOverride;
        }
      }
    }
  }

  void SwitchScene(SceneId id, int timerOverride = -1) {
    switchSceneRequest = new SwitchSceneRequest(deps, id, timerOverride);
    switchSceneIfRequired();
  }
};
