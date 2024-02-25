#pragma once

#include "../network/WifiManager.h"
#include "AbstractScene.h"

class BootupLogoScene : public AbstractScene {
private:
  WifiManager *wifiManager;
  ResourceImage *ri_logo = nullptr;
  SceneTimer *timer = nullptr;

public:
  explicit BootupLogoScene(SceneDeps deps) : AbstractScene(deps) {
    this->wifiManager = deps.mgr;
    deps.displayHAL->setBackgroundColor(0x000000);
    LV_LOG_INFO("Loading boot logo");
    ri_logo = KnownResourceImages::get_BTT_LOGO();
    LV_LOG_INFO("Boot logo loaded");
    // up to 15 seconds to connect to wifi (if there was saved config) before showing AP config scene
    timer = new SceneTimer(15 * 1000);
  }

  ~BootupLogoScene() override {
    delete ri_logo;
    delete timer;
  }

  SwitchSceneRequest *NextScene() override {
    if (wifiManager->isInConfigMode() && !timer->isCompleted()) {
      return new SwitchSceneRequest(deps, SceneId::APConfig);
    } else if (!wifiManager->isInConfigMode()) {
      // return new SwitchSceneRequest(deps, SceneId::Demo);
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }

  void Tick() override { ri_logo->tick(deps.displayHAL); }
};