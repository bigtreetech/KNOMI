#pragma once
#include "AbstractScene.h"

class APConfigScene : public AbstractScene {
private:
  ResourceImage *logo = nullptr, *apcfg = nullptr;

public:
  explicit APConfigScene(SceneDeps deps) : AbstractScene(deps) {
    deps.displayHAL->setBackgroundColor(0x000000);
    logo = KnownResourceImages::get_AP_Config_Back();
    apcfg = KnownResourceImages::get_AP_Config(0, -36);
  }

  ~APConfigScene() override {
    delete logo;
    delete apcfg;
    deps.displayHAL->setBackgroundColor(0xFFFFFF);
  }

  SwitchSceneRequest *NextScene() override {
    if (deps.mgr->isConnected()) {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }

  void Tick() override {
    logo->tick(deps.displayHAL);
    apcfg->tick(deps.displayHAL);
  }
};