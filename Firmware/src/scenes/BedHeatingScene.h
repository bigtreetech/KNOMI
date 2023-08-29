#pragma once
#include "AbstractScene.h"

LV_FONT_DECLARE(font_32)

class BedHeatingScene : public AbstractScene {
private:
  ResourceImage *ri_bed;
  lv_obj_t *label_bed_actual_temp;
  lv_obj_t *label_bed_target_temp;
  lv_style_t style_label_bed_actual_temp;
  lv_style_t style_label_bed_target_temp;

public:
  explicit BedHeatingScene(KlipperApi *api, WifiManager* mgr) : AbstractScene(api, mgr) {
    ri_bed = KnownResourceImages::get_bed_temp();
    init_label_heaterbed_actual_temp();
    init_label_heaterbed_target_temp();
  }

  ~BedHeatingScene() override {
    delete ri_bed;
    lv_obj_del(label_bed_actual_temp);
    lv_obj_del(label_bed_target_temp);
  }

  SwitchSceneRequest* NextScene() override {
    if (klipperApi->getBedActualTempValue() >= klipperApi->getBedTargetTempValue() && klipperApi->getBedTargetTemp() != 0 || klipperApi->getBedTargetTemp() == 0) {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::ExtruderHeating);
    }
    lv_label_set_text(label_bed_actual_temp, klipperApi->getBedActualTemp().c_str());
    lv_label_set_text(label_bed_target_temp, klipperApi->getBedTargetTemp().c_str());
    return nullptr;
  }

  void init_label_heaterbed_actual_temp() {
    label_bed_actual_temp = lv_label_create(lv_scr_act()); // 创建文字对象

    lv_style_init(&style_label_bed_actual_temp);
    lv_style_set_text_font(&style_label_bed_actual_temp, &font_32); // 设置字体样机及大小
    lv_style_set_text_color(&style_label_bed_actual_temp, lv_color_hex(0xFF0000)); // 设置样式文本字颜色

    lv_obj_add_style(label_bed_actual_temp, &style_label_bed_actual_temp, LV_PART_MAIN); // 将样式添加到文字对象中
    lv_label_set_text(label_bed_actual_temp, klipperApi->getBedActualTemp().c_str()); lv_obj_align(label_bed_actual_temp, LV_ALIGN_CENTER, 0, 75); // 居中显示
  }

  void init_label_heaterbed_target_temp() {
    label_bed_target_temp = lv_label_create(lv_scr_act()); // 创建文字对象

    lv_style_init(&style_label_bed_target_temp);
    lv_style_set_text_font(&style_label_bed_target_temp, &font_32); // 设置字体样机及大小
    lv_style_set_text_color(&style_label_bed_target_temp, lv_color_hex(0xFF0000)); // 设置样式文本字颜色

    lv_obj_add_style(label_bed_target_temp, &style_label_bed_target_temp, LV_PART_MAIN); // 将样式添加到文字对象中
    lv_label_set_text(label_bed_target_temp, klipperApi->getBedTargetTemp().c_str());
    lv_obj_align(label_bed_target_temp, LV_ALIGN_CENTER, 0, -75); // 居中显示
  }
};