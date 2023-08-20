#pragma once
#include "AbstractScene.h"

class LevelingScene : public AbstractScene {
private:
  ResourceImage *ri_leveling;

public:
  explicit LevelingScene(KlipperApi *api) : AbstractScene(api) {
    ri_leveling = KnownResourceImages::get_levelling();
  }

  ~LevelingScene() override { delete ri_leveling; }

  SwitchSceneRequest* NextScene() override {
    if (!klipperApi->isLeveling()) {
      return new SwitchSceneRequest(klipperApi, SceneId::Standby);
    }
    return nullptr;
  }
};