#pragma once
#include "AbstractScene.h"

class AfterPrintingScene : public AbstractScene {
private:
  ResourceImage *ri_after;
  SceneTimer *timer;

public:
  explicit AfterPrintingScene(SceneDeps deps) : AbstractScene(deps) {
    ri_after = KnownResourceImages::get_AfterPrinting();
    timer = new SceneTimer(5000);
  }
  ~AfterPrintingScene() override {
    delete ri_after;
    delete timer;
  }

  SwitchSceneRequest *NextScene() override {
    if (timer->isCompleted() && ri_after->isPlayedToEnd())
      return new SwitchSceneRequest(deps, SceneId::Standby);
    return nullptr;
  };

  void Tick() override { ri_after->tick(deps.displayHAL); }
};
