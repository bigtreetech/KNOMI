#pragma once
#include "../network/KnomiWebServer.h"
#include "AbstractScene.h"

class FirmwareUpdateScene : public AbstractScene {
private:
  KnomiWebServer *webServer;
  Arc* arc;
  TextLabel* text;

public:
  explicit FirmwareUpdateScene(SceneDeps deps) : AbstractScene(deps) {
    this->webServer = deps.webServer;
    text = new TextLabel(deps.styles, 48, 0, 0);
    arc = new Arc(deps.styles);
  }
  ~FirmwareUpdateScene() override {
    delete text;
    delete arc;
  }

  void Tick() override {
    if (webServer->getUpdateTotal() > 0) {
      auto value = (int16_t)(100.0 * webServer->getUpdateDone() / webServer->getUpdateTotal());
      String result = String(value) + "%";
      text->setText(result);
      arc->setProgress(value);
    } else {
      text->setText("done");
    }
    text->tick(deps.displayHAL);
    arc->tick(deps.displayHAL);
  }

  SwitchSceneRequest *NextScene() override {
    if (!webServer->isUpdateInProgress()) {
      return new SwitchSceneRequest(deps, SceneId::Standby, 0);
    }
    return nullptr;
  }
};
