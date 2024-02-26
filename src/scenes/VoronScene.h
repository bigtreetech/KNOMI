#pragma once
#include "AbstractScene.h"
#include "StandbyScene.h"

class VoronScene : public AbstractScene {
private:
  ResourceImage *ri_voron;

public:
  explicit VoronScene(SceneDeps deps) : AbstractScene(deps) { ri_voron = KnownResourceImages::get_Voron(); }
  ~VoronScene() override { delete ri_voron; }

  SwitchSceneRequest *NextScene() override {
    if (deps.klipperApi->isPrinting())
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
    else if (ri_voron->isPlayedToEnd())
      return new SwitchSceneRequest(deps, SceneId::Standby);
    else
      return nullptr;
  };

  void Tick() override { ri_voron->tick(deps.displayHAL); }
};
