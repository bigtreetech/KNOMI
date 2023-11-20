#pragma once
#include "../network/KnomiWebServer.h"
#include "AbstractScene.h"

class FirmwareUpdateScene : public AbstractScene {
private:
  UpdateProgress *progress;
  Arc *arc;
  TextLabel *text;

public:
  explicit FirmwareUpdateScene(SceneDeps deps) : AbstractScene(deps) {
    progress = deps.progress;
    progress->canStartUpdate = true;
    text = new TextLabel(deps.styles, fontSize::small, 0, 0);
    arc = new Arc(deps.styles);
  }
  ~FirmwareUpdateScene() override {
    progress->canStartUpdate = false;
    delete text;
    delete arc;
  }

  void Tick() override {
    if (progress->total > 0) {
      auto value = (int16_t)(100.0 * progress->current / progress->total);
      String result = String(value) + "%";
      text->setText(result);
      arc->setProgress(value);
    } else {
      String result = "done";
      text->setText(result);
      arc->setProgress(100);
    }
    text->tick(deps.displayHAL);
    arc->tick(deps.displayHAL);
  }

  SwitchSceneRequest *NextScene() override {
    if (!progress->isInProgress) {
      return new SwitchSceneRequest(deps, SceneId::Standby, 0);
    }
    return nullptr;
  }
};
