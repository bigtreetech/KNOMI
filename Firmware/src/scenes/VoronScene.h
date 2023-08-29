#pragma once
#include "AbstractScene.h"
#include "StandbyScene.h"

class VoronScene : public AbstractScene {
private:
  ResourceImage *ri_voron;

public:
  explicit VoronScene(KlipperApi *api, WifiManager* mgr) : AbstractScene(api, mgr) {

    ri_voron = KnownResourceImages::get_Voron();
  }
  ~VoronScene() override { delete ri_voron; }

  SwitchSceneRequest *NextScene() override {
    if (klipperApi->isPrinting())
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Homing);
    else if (klipperApi->isLeveling())
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Leveling);
    else if (klipperApi->isPrinting())
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::BedHeating);
    else
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Standby);
  };
};
