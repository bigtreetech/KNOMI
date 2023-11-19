#pragma once
#include "AbstractScene.h"

class APConfigScene : public AbstractScene {
private:
  ResourceImage *logo = nullptr, *apcfg = nullptr;
  bool logoTicked = false;

public:
  explicit APConfigScene(SceneDeps deps) : AbstractScene(deps) {
    deps.displayHAL->setBackgroundColor(deps.styles->getBackgroundColor());
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
    if (!logoTicked) {
      logo->tick(deps.displayHAL);
      logoTicked = true;
    }
    apcfg->tick(deps.displayHAL);
  }
};