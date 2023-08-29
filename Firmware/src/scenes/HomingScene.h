#pragma once
#include "AbstractScene.h"
#include "StandbyScene.h"

class HomingScene : public AbstractScene {
private:
  ResourceImage *ri_home;

public:
  explicit HomingScene(KlipperApi *api, WifiManager* mgr) : AbstractScene(api, mgr) {
    ri_home = KnownResourceImages::get_Home();
  }

  ~HomingScene() override { delete ri_home; }

  SwitchSceneRequest* NextScene() override {
      if (!klipperApi->isHoming()) {
        return new SwitchSceneRequest(klipperApi, mgr, SceneId::Standby, 1);
      }
      return nullptr;
  }
};