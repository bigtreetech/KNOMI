#pragma once
#include "AbstractScene.h"

class BeforePrintingScene : public AbstractScene {
private:
  ResourceImage *ri_before;

public:
  explicit BeforePrintingScene(KlipperApi *api, WifiManager* mgr) : AbstractScene(api, mgr) {
    ri_before = KnownResourceImages::get_BeforePrinting();
  }

  ~BeforePrintingScene() override { delete ri_before; }

  SwitchSceneRequest *NextScene() override {
    return new SwitchSceneRequest(klipperApi, mgr, SceneId::Printing);
  }
};