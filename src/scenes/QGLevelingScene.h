#pragma once
#include "AbstractScene.h"

class QGLevelingScene : public AbstractScene {
private:
  ResourceImage *ri_leveling;

public:
  explicit QGLevelingScene(SceneDeps deps) : AbstractScene(deps) { ri_leveling = KnownResourceImages::get_qgling(); }

  ~QGLevelingScene() override { delete ri_leveling; }

  SwitchSceneRequest *NextScene() override {
    if (!deps.klipperApi->isQGLeveling() && ri_leveling->isPlayedToEnd()) {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }

  void Tick() override { ri_leveling->tick(deps.displayHAL); }
};