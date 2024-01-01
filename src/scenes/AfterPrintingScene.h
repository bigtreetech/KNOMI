#pragma once
#include "AbstractScene.h"

class AfterPrintingScene : public AbstractScene {
private:
  ResourceImage *ri_after;

public:
  explicit AfterPrintingScene(SceneDeps deps) : AbstractScene(deps) {
    ri_after = KnownResourceImages::get_AfterPrinting();
  }
  ~AfterPrintingScene() override { delete ri_after; }

  SwitchSceneRequest *NextScene() override { return new SwitchSceneRequest(deps, SceneId::Standby, 10); };

  void Tick() override { ri_after->tick(deps.displayHAL); }
};
