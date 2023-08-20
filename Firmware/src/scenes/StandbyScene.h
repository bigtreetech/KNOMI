#pragma once
#include "AbstractScene.h"
#include "SwitchSceneRequest.h"
#include "VoronScene.h"

class StandbyScene : public AbstractScene {
private:
  ResourceImage *ri_standby;

public:
  explicit StandbyScene(KlipperApi *api) : AbstractScene(api) {
    ri_standby = KnownResourceImages::get_Standby();
  }

  ~StandbyScene() override { delete ri_standby; }

  SwitchSceneRequest* NextScene() override {
    if (klipperApi->isHoming()) {
      return new SwitchSceneRequest(klipperApi, SceneId::Homing);
    } else if (klipperApi->isLeveling()) {
      return new SwitchSceneRequest(klipperApi, SceneId::Leveling);
    } else if (klipperApi->isPrinting()) {
      return new SwitchSceneRequest(klipperApi, SceneId::BedHeating);
    } else {
      return new SwitchSceneRequest(klipperApi, SceneId::Voron);
    }
  }
};