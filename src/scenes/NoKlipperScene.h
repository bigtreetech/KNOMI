#pragma once
#include "AbstractScene.h"

class NoKlipperScene : public AbstractScene {
private:
  ResourceImage *ri_disconnect;

public:
  explicit NoKlipperScene(SceneDeps deps) : AbstractScene(deps) {
    ri_disconnect = KnownResourceImages::get_Disconnect_Back();
  }

  ~NoKlipperScene() override { delete ri_disconnect; }
  SwitchSceneRequest *NextScene() override {
    if (!deps.klipperApi->isKlipperNotAvailable()) {
      return new SwitchSceneRequest(deps, SceneId::Standby, 0);
    }
    return nullptr;
  }

  void Tick() override { ri_disconnect->tick(deps.displayHAL); }
};