#pragma once
#include "AbstractScene.h"

class PrintingScene : public AbstractScene {
private:
  ResourceImage *ri_printing;

public:
  explicit PrintingScene(KlipperApi *api) : AbstractScene(api) {
    ri_printing = KnownResourceImages::get_Printing();
  }
  ~PrintingScene() override { delete ri_printing; }

  SwitchSceneRequest *NextScene() override {
    if (klipperApi->isPrinting()) {
      if (klipperApi->getProgressData() == 100) {
        return new SwitchSceneRequest(klipperApi, SceneId::Printing100Percent, 7);
      } else if (klipperApi->getProgressData() >= 1) {
        return new SwitchSceneRequest(klipperApi, SceneId::Printing1Percent);
      }
    } else {
      return new SwitchSceneRequest(klipperApi, SceneId::Standby);
    }
    return nullptr;
  }
};