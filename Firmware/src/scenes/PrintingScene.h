#pragma once
#include "AbstractScene.h"

class PrintingScene : public AbstractScene {
private:
  ResourceImage *ri_printing;

public:
  explicit PrintingScene(KlipperApi *api, WifiManager* mgr) : AbstractScene(api, mgr) {
    ri_printing = KnownResourceImages::get_Printing();
  }
  ~PrintingScene() override { delete ri_printing; }

  SwitchSceneRequest *NextScene() override {
    if (klipperApi->isPrinting()) {
      if (klipperApi->getProgressData() == 100) {
        return new SwitchSceneRequest(klipperApi, mgr, SceneId::Printing100Percent, 7);
      } else if (klipperApi->getProgressData() >= 1) {
        return new SwitchSceneRequest(klipperApi, mgr, SceneId::Printing1Percent);
      }
    } else {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Standby);
    }
    return nullptr;
  }
};