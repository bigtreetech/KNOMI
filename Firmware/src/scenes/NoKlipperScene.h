#pragma once
#include "AbstractScene.h"

LV_FONT_DECLARE(font_20);

class NoKlipperScene : public AbstractScene {
private:
  ResourceImage *ri_disconnect;

public:
  explicit NoKlipperScene(KlipperApi *api, WifiManager* mgr) : AbstractScene(api, mgr) {
    ri_disconnect = KnownResourceImages::get_Disconnect_Back();
  }

  ~NoKlipperScene() override { delete ri_disconnect; }
  SwitchSceneRequest *NextScene() override {
    if (!klipperApi->isKlipperNotAvailable()) {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Standby, 0);
    }
    return nullptr;
  }
};