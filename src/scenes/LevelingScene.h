#pragma once
#include "AbstractScene.h"

class LevelingScene : public AbstractScene {
private:
  ResourceImage *ri_leveling;

public:
  explicit LevelingScene(SceneDeps deps) : AbstractScene(deps) { ri_leveling = KnownResourceImages::get_probing(); }

  ~LevelingScene() override { delete ri_leveling; }

  SwitchSceneRequest *NextScene() override {
    if (!deps.klipperApi->isLeveling() && ri_leveling->isPlayedToEnd()) {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }

  void Tick() override { ri_leveling->tick(deps.displayHAL); }
};