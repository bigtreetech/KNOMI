#pragma once
#include "AbstractScene.h"

class Printing1PercentScene : public AbstractScene {
public:
  TextLabel *text;
  Arc *arc;
  SceneTimer *timer = nullptr;

  explicit Printing1PercentScene(SceneDeps deps) : AbstractScene(deps) {
    text = new TextLabel(deps.styles, fontSize::large, 0, 0);
    arc = new Arc(deps.styles);
  }
  ~Printing1PercentScene() override {
    delete text;
    delete arc;
    delete timer;
  }

  SwitchSceneRequest *NextScene() override {
    if (deps.klipperApi->isPrinting()) {
      if (deps.klipperApi->getProgressData() == 100) {
        if (timer == nullptr) {
          timer = new SceneTimer(7000);
        } else if (timer->isCompleted()) {
          return new SwitchSceneRequest(deps, SceneId::Printing100Percent);
        }
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
