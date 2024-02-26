#pragma once
#include "AbstractScene.h"

class BeforePrintingScene : public AbstractScene {
private:
  ResourceImage *ri_before;

public:
  explicit BeforePrintingScene(SceneDeps deps) : AbstractScene(deps) {
    ri_before = KnownResourceImages::get_BeforePrinting();
  }

  ~BeforePrintingScene() override { delete ri_before; }

  SwitchSceneRequest *NextScene() override {
    if (ri_before->isPlayedToEnd())
      return new SwitchSceneRequest(deps, SceneId::Printing);
    return nullptr;
  }

  void Tick() override { ri_before->tick(deps.displayHAL); }
};