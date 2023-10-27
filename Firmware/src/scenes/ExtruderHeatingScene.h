#pragma once
#include "AbstractScene.h"
#include "Styles.h"

LV_FONT_DECLARE(font_32)

class ExtruderHeatingScene : public AbstractScene {
private:
  ResourceImage *ri_ext;

  lv_obj_t *label_ext_actual_temp;
  lv_obj_t *label_ext_target_temp;

public:
  explicit ExtruderHeatingScene(SceneDeps deps) : AbstractScene(deps) {
    ri_ext = KnownResourceImages::get_ext_temp();
    init_label_extruder_actual_temp();
    init_label_extruder_target_temp();
  }

  ~ExtruderHeatingScene() override {
    delete ri_ext;

    lv_obj_del(label_ext_actual_temp);
    lv_obj_del(label_ext_target_temp);
  }

  SwitchSceneRequest *NextScene() override {
    if ((deps.klipperApi->getExtruderActualTempValue() >= deps.klipperApi->getExtruderTargetTempValue()) &&
        (deps.klipperApi->getExtruderTargetTempValue() != 0)) {
      // heated fully...

      if (!deps.klipperApi->isPrinting()) {
        return new SwitchSceneRequest(deps, SceneId::Standby);
      } else {
        return new SwitchSceneRequest(deps, SceneId::BeforePrint);
      }
    } else {
      if (deps.klipperApi->getExtruderTargetTempValue() == 0) {
        return new SwitchSceneRequest(deps, SceneId::Printing);
      }
    }

    lv_label_set_text(label_ext_actual_temp, deps.klipperApi->getExtruderActualTemp().c_str());
    lv_label_set_text(label_ext_target_temp, deps.klipperApi->getExtruderTargetTemp().c_str());

    return nullptr;
  }

  void init_label_extruder_actual_temp() {
    label_ext_actual_temp = lv_label_create(lv_scr_act());

    lv_obj_add_style(label_ext_actual_temp, Styles::getRedText32(), LV_PART_MAIN);
    lv_label_set_text(label_ext_actual_temp, deps.klipperApi->getExtruderActualTemp().c_str());
    lv_obj_align(label_ext_actual_temp, LV_ALIGN_CENTER, 0, 75);
  }

  void init_label_extruder_target_temp() {
    label_ext_target_temp = lv_label_create(lv_scr_act());

    lv_obj_add_style(label_ext_target_temp, Styles::getRedText32(), LV_PART_MAIN);
    lv_label_set_text(label_ext_target_temp, deps.klipperApi->getExtruderTargetTemp().c_str());
    lv_obj_align(label_ext_target_temp, LV_ALIGN_CENTER, 0, -75);
  }
};