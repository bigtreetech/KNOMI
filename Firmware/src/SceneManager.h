#pragma once
#include "lvgl.h"
#include "network/WifiManager.h"
#include "scenes/AbstractScene.h"
#include "scenes/BootupLogo.h"
#include "scenes/SwitchSceneRequest.h"

class SceneManager {
private:
  uint8_t timer_contne = 0;
  AbstractScene *currentScene = nullptr;
  SceneId currentSceneId;
  SwitchSceneRequest *switchSceneRequest = nullptr;
  SceneDeps deps;

public:
  explicit SceneManager(KnomiWebServer *webServer, KlipperApi *klipperApi, WifiManager *manager)
      : deps(klipperApi, manager, webServer) {
    this->currentScene = new BootupLogoScene(deps);
    this->currentSceneId = SceneId::BootupLogo;
  }

  SceneId getCurrentSceneId() { return currentSceneId; }

  void SwitchSceneIfRequired() {
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

  void Timer() {
    if (currentScene != nullptr) {
      currentScene->RefreshData();
    }

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
    switchSceneRequest = new SwitchSceneRequest(deps, id, timerOverride);
    SwitchSceneIfRequired();
  }
};
