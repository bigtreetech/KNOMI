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

    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_hex(0x000000));

    LV_LOG_INFO(("Loading boot logo, free heap = " + String(esp_get_free_heap_size())).c_str());
    ri_logo = KnownResourceImages::get_BTT_LOGO();
    LV_LOG_INFO("Boot logo loaded");

    LV_LOG_INFO("Creating styles");
    lv_style_init(&style_spinner_open);
    lv_style_set_arc_color(&style_spinner_open, lv_color_hex(0xFF0000));
    lv_style_set_arc_width(&style_spinner_open, 16);

    lv_style_init(&style_bc_spinner_open);
    lv_style_set_arc_color(&style_bc_spinner_open, lv_color_hex(0x000000));
    lv_style_set_arc_width(&style_bc_spinner_open, 16);
    LV_LOG_INFO("Style loaded");

    open_anim_arc = lv_spinner_create(lv_scr_act(), 1000, 45);
    LV_LOG_INFO("Spinner created");

    lv_obj_add_style(open_anim_arc, &style_spinner_open, LV_PART_INDICATOR);
    lv_obj_add_style(open_anim_arc, &style_bc_spinner_open, LV_PART_MAIN);

    lv_obj_set_size(open_anim_arc, 240, 240);
    lv_obj_align(open_anim_arc, LV_ALIGN_CENTER, 0, 0);
    LV_LOG_INFO("Spinner setup");
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