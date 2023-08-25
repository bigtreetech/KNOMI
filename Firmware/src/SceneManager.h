#pragma once
#include "scenes/AbstractScene.h"
#include "scenes/SwitchSceneRequest.h"
#include "scenes/BootupLogo.h"
#include "network/WifiManager.h"
#include "lvgl.h"

class SceneManager {
private:
  uint8_t timer_contne = 0;
  lv_timer_t *update_timer = nullptr;
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
      delete currentScene;
      currentScene = nullptr;
      update_timer = lv_timer_create(sceneTimer, 0, this);
      lv_timer_set_repeat_count(update_timer, 1);
    }
  }

  void Timer() {
    if (timer_contne > 0)
      timer_contne--;

    if (!manager->isInConfigMode() && timer_contne == 0) {
      if (currentScene != nullptr) {
        timer_contne = 5;
        switchSceneRequest = currentScene->NextScene();
        if (switchSceneRequest != nullptr && switchSceneRequest->timerOverride >= 0) {
          timer_contne = switchSceneRequest->timerOverride;
        }
      }
    }

  }

  static void sceneTimer(lv_timer_t* timer) {
    auto* that = (SceneManager*) timer->user_data;
    that->currentScene = that->switchSceneRequest->Provide();
    delete that->switchSceneRequest;
    that->switchSceneRequest = nullptr;
  }
  void SwitchScene(SceneId id, int timerOverride = -1) {
    switchSceneRequest = new SwitchSceneRequest(klipperApi, id, timerOverride);
  }
};

