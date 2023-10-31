#pragma once
#include "AbstractScene.h"
#include "StandbyScene.h"

class DemoScene : public AbstractScene {
private:
  Arc *arc;
  TextLabel *text;
  int progress = 0;

public:
  explicit DemoScene(SceneDeps deps) : AbstractScene(deps) {
    this->arc = new Arc(deps.styles);
    this->text = new TextLabel(deps.styles, 32, 0, 0);
  }

  ~DemoScene() override {}

  SwitchSceneRequest *NextScene() override { return nullptr; };

  void Tick() override {
    progress = (progress + 1) % 100;
    arc->setProgress(progress);
    String result = progress + String("%");
    text->setText(result);
    arc->tick(deps.displayHAL);
    text->tick(deps.displayHAL);
  }
};
