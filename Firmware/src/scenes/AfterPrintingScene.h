#pragma once
#include "AbstractScene.h"

class AfterPrintingScene : public AbstractScene {
private:
  ResourceImage *ri_after;

public:
  explicit AfterPrintingScene(KlipperApi *api) : AbstractScene(api) {
    ri_after = KnownResourceImages::get_AfterPrinting();
  }
  ~AfterPrintingScene() override { delete ri_after; }

  SwitchSceneRequest *NextScene() override {
    return new SwitchSceneRequest(klipperApi, SceneId::Standby, 10);
  };
};
