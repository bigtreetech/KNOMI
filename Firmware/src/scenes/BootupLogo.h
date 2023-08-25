#pragma once

#include "../network/WifiManager.h"
#include "AbstractScene.h"

class BootupLogoScene : public AbstractScene {
private:
  WifiManager *wifiManager;
  ResourceImage *ri_logo = nullptr;
  lv_style_t style_spinner_open;
  lv_style_t style_bc_spinner_open;
  lv_obj_t *open_anim_arc;

public:
  explicit BootupLogoScene(KlipperApi *api, WifiManager *mgr)
      : AbstractScene(api) {
    this->wifiManager = mgr;
    if (!wifiManager->isInConfigMode()) {
      wifiManager->connectToWiFi();
    }

    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_hex(0x000000));
    ri_logo = KnownResourceImages::get_BTT_LOGO();

    lv_style_set_arc_color(&style_spinner_open, lv_color_hex(0xFF0000));
    lv_style_set_arc_width(&style_spinner_open, 16);

    lv_style_set_arc_color(&style_bc_spinner_open, lv_color_hex(0x000000));
    lv_style_set_arc_width(&style_bc_spinner_open, 16);

    open_anim_arc = lv_spinner_create(lv_scr_act(), 1000, 45);

    lv_obj_add_style(open_anim_arc, &style_spinner_open, LV_PART_INDICATOR);
    lv_obj_add_style(open_anim_arc, &style_bc_spinner_open, LV_PART_MAIN);

    lv_obj_set_size(open_anim_arc, 240, 240);
    lv_obj_align(open_anim_arc, LV_ALIGN_CENTER, 0, 0);
  }

  ~BootupLogoScene() override {
    lv_obj_del(open_anim_arc);
    delete ri_logo;
  }

  SwitchSceneRequest *NextScene() override {
    if (wifiManager->isInConfigMode()) {
      return new SwitchSceneRequest(klipperApi, SceneId::APConfig, 0);
    } else if (wifiManager->isConnected()) {
      return new SwitchSceneRequest(klipperApi, SceneId::Standby);
    }
    return nullptr;
  }
};