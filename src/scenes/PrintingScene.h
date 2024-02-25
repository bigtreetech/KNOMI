#pragma once
#include "AbstractScene.h"

class PrintingScene : public AbstractScene {
private:
  ResourceImage *ri_printing;

public:
  explicit PrintingScene(SceneDeps deps) : AbstractScene(deps) { ri_printing = KnownResourceImages::get_Printing(); }
  ~PrintingScene() override { delete ri_printing; }

  SwitchSceneRequest *NextScene() override {
    if (!ri_printing->isPlayedToEnd())
      return nullptr;

    if (deps.klipperApi->isPrinting()) {
      if (deps.klipperApi->getProgressData() == 100) {
        return new SwitchSceneRequest(deps, SceneId::Printing100Percent);
      } else if (deps.klipperApi->getProgressData() >= 1) {
        return new SwitchSceneRequest(deps, SceneId::Printing1Percent);
      }
    } else {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }

  void Tick() override { ri_printing->tick(deps.displayHAL); }
};