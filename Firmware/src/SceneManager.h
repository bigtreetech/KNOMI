#pragma once
#include "scenes/AbstractScene.h"
#include "scenes/SwitchSceneRequest.h"
#include "scenes/BootupLogo.h"
#include "network/WifiManager.h"
#include "lvgl.h"

class SceneManager {
private:
  uint8_t timer_contne = 0;
  AbstractScene* currentScene = nullptr;
  SwitchSceneRequest* switchSceneRequest = nullptr;
  KlipperApi *klipperApi = nullptr;
  WifiManager *manager = nullptr;

public:
  explicit SceneManager(KlipperApi* klipperApi, WifiManager* manager) {
    this->klipperApi = klipperApi;
    this->manager = manager;
    this->currentScene = new BootupLogoScene(klipperApi, manager);

  }

  void SwitchSceneIfRequired() {
    if (switchSceneRequest != nullptr) {
      LV_LOG_INFO("Deleting current scene");
      delete currentScene;
      currentScene = nullptr;
      LV_LOG_INFO((String("Switching scene to ") + String(switchSceneRequest->id)).c_str());
      currentScene = switchSceneRequest->Provide();
      delete switchSceneRequest;
      switchSceneRequest = nullptr;
    }
  }

  void Timer() {
    if (timer_contne > 0)
      timer_contne--;

    if (timer_contne == 0) {
      if (currentScene != nullptr) {
        timer_contne = 5;
        switchSceneRequest = currentScene->NextScene();
        if (switchSceneRequest != nullptr && switchSceneRequest->timerOverride >= 0) {
          timer_contne = switchSceneRequest->timerOverride;
        }
      }
    }

  }

  void SwitchScene(SceneId id, int timerOverride = -1) {
    switchSceneRequest = new SwitchSceneRequest(klipperApi, manager, id, timerOverride);
  }
};

