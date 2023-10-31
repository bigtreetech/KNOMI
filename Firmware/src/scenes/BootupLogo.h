#pragma once

#include "../network/WifiManager.h"
#include "AbstractScene.h"

class BootupLogoScene : public AbstractScene {
private:
  WifiManager *wifiManager;
  ResourceImage *ri_logo = nullptr; /* TODO
   lv_style_t style_spinner_open;
   lv_style_t style_bc_spinner_open;
   lv_obj_t *open_anim_arc;*/

public:
  explicit BootupLogoScene(SceneDeps deps) : AbstractScene(deps) {
    this->wifiManager = deps.mgr;
    deps.displayHAL->setBackgroundColor(0x000000);
    LV_LOG_INFO("Loading boot logo");
    // ri_logo = KnownResourceImages::get_BTT_LOGO();
    LV_LOG_INFO("Boot logo loaded");

    /* TODO
    LV_LOG_INFO("Creating styles");
    lv_style_init(&style_spinner_open);
    lv_style_set_arc_color(&style_spinner_open, lv_color_hex(deps.styles->getAccentColor()));
    lv_style_set_arc_rounded(&style_spinner_open, true);
    lv_style_set_arc_width(&style_spinner_open, 16);

    lv_style_init(&style_bc_spinner_open);
    lv_style_set_arc_color(&style_bc_spinner_open, lv_color_hex(0x000000));
    lv_style_set_arc_rounded(&style_bc_spinner_open, true);
    lv_style_set_arc_width(&style_bc_spinner_open, 16);
    LV_LOG_INFO("Style loaded");

    open_anim_arc = lv_spinner_create(lv_scr_act(), 1000, 45);
    LV_LOG_INFO("Spinner created");

    lv_obj_add_style(open_anim_arc, &style_spinner_open, LV_PART_INDICATOR);
    lv_obj_add_style(open_anim_arc, &style_bc_spinner_open, LV_PART_MAIN);

    lv_obj_set_size(open_anim_arc, 240, 240);
    lv_obj_align(open_anim_arc, LV_ALIGN_CENTER, 0, 0);
    LV_LOG_INFO("Spinner setup");
     */
  }

  ~BootupLogoScene() override {
    // TODO lv_obj_del(open_anim_arc);
    // delete ri_logo;
  }

  SwitchSceneRequest *NextScene() override {
    if (wifiManager->isInConfigMode()) {
      return new SwitchSceneRequest(deps, SceneId::APConfig, 0);
    } else if (wifiManager->isConnected()) {
      return new SwitchSceneRequest(deps, SceneId::Standby);
    }
    return nullptr;
  }

  void Tick() override {
    // ri_logo->tick(deps.displayHAL);
  }
};