#pragma once
#include "AbstractScene.h"
#include "StandbyScene.h"

class HomingScene : public AbstractScene {
private:
  ResourceImage *ri_home;

public:
  explicit HomingScene(SceneDeps deps) : AbstractScene(deps) { ri_home = KnownResourceImages::get_Home(); }

  ~HomingScene() override { delete ri_home; }

  SwitchSceneRequest *NextScene() override {
    if (!deps.klipperApi->isHoming() && ri_home->isPlayedToEnd()) {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }

  void Tick() override { ri_home->tick(deps.displayHAL); }
};