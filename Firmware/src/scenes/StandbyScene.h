#pragma once
#include "AbstractScene.h"
#include "SwitchSceneRequest.h"
#include "VoronScene.h"

class StandbyScene : public AbstractScene {
private:
  ResourceImage *ri_standby;

public:
  explicit StandbyScene(SceneDeps deps) : AbstractScene(deps) { ri_standby = KnownResourceImages::get_Standby(); }

  ~StandbyScene() override { delete ri_standby; }

  SwitchSceneRequest *NextScene() override {
    if (deps.klipperApi->isHoming()) {
      return new SwitchSceneRequest(deps, SceneId::Homing);
    } else if (deps.klipperApi->isLeveling()) {
      return new SwitchSceneRequest(deps, SceneId::Leveling);
    } else if (deps.klipperApi->isPrinting()) {
      return new SwitchSceneRequest(deps, SceneId::BedHeating);
    } else {
      return new SwitchSceneRequest(deps, SceneId::Voron);
    }
  }
};