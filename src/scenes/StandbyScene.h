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
    if (deps.klipperApi->isHoming())
      return new SwitchSceneRequest(deps, SceneId::Homing);
    else if (deps.klipperApi->isLeveling())
      return new SwitchSceneRequest(deps, SceneId::Leveling);
    else if (deps.klipperApi->isQGLeveling())
      return new SwitchSceneRequest(deps, SceneId::QGLeveling);
    else if (deps.klipperApi->isHeatingBed())
      return new SwitchSceneRequest(deps, SceneId::BedHeating);
    else if (deps.klipperApi->isHeatingNozzle())
      return new SwitchSceneRequest(deps, SceneId::ExtruderHeating);
    else if (deps.klipperApi->isPrinting())
      return new SwitchSceneRequest(deps, SceneId::BeforePrint);
    else if (ri_standby->isPlayedToEnd())
      return new SwitchSceneRequest(deps, SceneId::Voron);
    else
      return nullptr;
  }

  void Tick() override { ri_standby->tick(deps.displayHAL); }
};