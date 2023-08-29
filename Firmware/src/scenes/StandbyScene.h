#pragma once
#include "AbstractScene.h"
#include "SwitchSceneRequest.h"
#include "VoronScene.h"

class StandbyScene : public AbstractScene {
private:
  ResourceImage *ri_standby;

public:
  explicit StandbyScene(KlipperApi *api, WifiManager* mgr) : AbstractScene(api, mgr) {
    ri_standby = KnownResourceImages::get_Standby();
  }

  ~StandbyScene() override { delete ri_standby; }

  SwitchSceneRequest* NextScene() override {
    if (klipperApi->isHoming()) {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Homing);
    } else if (klipperApi->isLeveling()) {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Leveling);
    } else if (klipperApi->isPrinting()) {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::BedHeating);
    } else {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Voron);
    }
  }
};