#pragma once
#include "AbstractScene.h"

class APConfigScene : public AbstractScene {
private:
  ResourceImage *logo = nullptr, *apcfg = nullptr;

public:
  explicit APConfigScene(KlipperApi *api) : AbstractScene(api) {
    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_hex(0xFFFFFF));
    logo = KnownResourceImages::get_AP_Config_Back();
    apcfg = KnownResourceImages::get_AP_Config(0, -36);
  }

  ~APConfigScene() override {
    delete logo;
    delete apcfg;
  }

  SwitchSceneRequest *NextScene() override {
    return nullptr;
  }
};