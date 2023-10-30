#pragma once
#include "AbstractScene.h"

class BedHeatingScene : public AbstractScene {
private:
  ResourceImage *ri_bed;

public:
  explicit BedHeatingScene(SceneDeps deps) : AbstractScene(deps) {
    ri_bed = KnownResourceImages::get_bed_temp();
    /* TODO
    label_bed_actual_temp = lv_label_create(lv_scr_act());
    lv_obj_add_style(label_bed_actual_temp, deps.styles->getAccentText32(), LV_PART_MAIN);
    lv_label_set_text(label_bed_actual_temp, deps.klipperApi->getBedActualTemp().c_str());
    lv_obj_align(label_bed_actual_temp, LV_ALIGN_CENTER, 0, 75);

    label_bed_target_temp = lv_label_create(lv_scr_act());
    lv_obj_add_style(label_bed_target_temp, deps.styles->getAccentText32(), LV_PART_MAIN);
    lv_label_set_text(label_bed_target_temp, deps.klipperApi->getBedTargetTemp().c_str());
    lv_obj_align(label_bed_target_temp, LV_ALIGN_CENTER, 0, -75);
     */
  }

  ~BedHeatingScene() override {
    delete ri_bed;
    /* TODO
    lv_obj_del(label_bed_actual_temp);
    lv_obj_del(label_bed_target_temp);
     */
  }

  SwitchSceneRequest *NextScene() override {
    if (deps.klipperApi->getBedActualTempValue() >= deps.klipperApi->getBedTargetTempValue() &&
            deps.klipperApi->getBedTargetTemp() != 0 ||
        deps.klipperApi->getBedTargetTemp() == 0) {
      return new SwitchSceneRequest(deps, SceneId::ExtruderHeating);
    }
    /* TODO
    lv_label_set_text(label_bed_actual_temp, deps.klipperApi->getBedActualTemp().c_str());
    lv_label_set_text(label_bed_target_temp, deps.klipperApi->getBedTargetTemp().c_str()); */
    return nullptr;
  }
};