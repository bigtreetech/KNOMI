#pragma once
#include "AbstractScene.h"

class Printing1PercentScene : public AbstractScene {
public:
  TextLabel* text;
  Arc* arc;

  explicit Printing1PercentScene(SceneDeps deps) : AbstractScene(deps) {
    text = new TextLabel(deps.styles, 48, 0, 0);
    arc = new Arc(deps.styles);
  }
  ~Printing1PercentScene() override {
    delete text;
    delete arc;
  }

  SwitchSceneRequest *NextScene() override {
    if (deps.klipperApi->isPrinting()) {
      if (deps.klipperApi->getProgressData() == 100) {
        return new SwitchSceneRequest(deps, SceneId::Printing100Percent, 7);
      }
    } else {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    String result = String(deps.klipperApi->getProgressData()) + "%";
    text->setText(result);
    arc->setProgress(deps.klipperApi->getProgressData());

    return nullptr;
  }

  void Tick() override {
    arc->tick(deps.displayHAL);
    text->tick(deps.displayHAL);
  }
};
