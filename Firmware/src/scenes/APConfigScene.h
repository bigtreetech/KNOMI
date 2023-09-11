#pragma once
#include "AbstractScene.h"

class APConfigScene : public AbstractScene {
private:
  ResourceImage *logo = nullptr, *apcfg = nullptr;

public:
  explicit APConfigScene(SceneDeps deps) : AbstractScene(deps) {
    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_hex(0xFFFFFF));
    logo = KnownResourceImages::get_AP_Config_Back();
    apcfg = KnownResourceImages::get_AP_Config(0, -36);
  }

  ~APConfigScene() override {
    delete logo;
    delete apcfg;
    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_hex(0x000000));
  }

  SwitchSceneRequest *NextScene() override {
    if (deps.mgr->isConnected()) {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }
};