#pragma once
#include "AbstractScene.h"
#include "StandbyScene.h"

class VoronScene : public AbstractScene {
private:
  ResourceImage *ri_voron;

public:
  explicit VoronScene(KlipperApi *api) : AbstractScene(api) {

    ri_voron = KnownResourceImages::get_Voron();
  }
  ~VoronScene() override { delete ri_voron; }

  SwitchSceneRequest *NextScene() override {
    if (klipperApi->isPrinting())
      return new SwitchSceneRequest(klipperApi, SceneId::Homing);
    else if (klipperApi->isLeveling())
      return new SwitchSceneRequest(klipperApi, SceneId::Leveling);
    else if (klipperApi->isPrinting())
      return new SwitchSceneRequest(klipperApi, SceneId::BedHeating);
    else
      return new SwitchSceneRequest(klipperApi, SceneId::Standby);
  };
};
