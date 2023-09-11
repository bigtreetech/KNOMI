#pragma once
#include "AbstractScene.h"

class LevelingScene : public AbstractScene {
private:
  ResourceImage *ri_leveling;

public:
  explicit LevelingScene(SceneDeps deps) : AbstractScene(deps) {
    ri_leveling = KnownResourceImages::get_levelling();
  }

  ~LevelingScene() override { delete ri_leveling; }

  SwitchSceneRequest* NextScene() override {
    if (!deps.klipperApi->isLeveling()) {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }
};