#pragma once
#include "AbstractScene.h"

LV_FONT_DECLARE(font_32)

class ExtruderHeatingScene : public AbstractScene {
private:
  ResourceImage *ri_ext;

  lv_obj_t *label_ext_actual_temp;
  lv_obj_t *label_ext_target_temp;
  lv_style_t style_label_ext_actual_temp;
  lv_style_t style_label_ext_target_temp;

public:
  explicit ExtruderHeatingScene(KlipperApi *api) : AbstractScene(api) {
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
    if ((klipperApi->getExtruderActualTempValue() >=
         klipperApi->getExtruderTargetTempValue()) &&
        (klipperApi->getExtruderTargetTempValue() != 0)) {
      // heated fully...

      if (!klipperApi->isPrinting()) {
        return new SwitchSceneRequest(klipperApi, SceneId::Standby);
      } else {
        return new SwitchSceneRequest(klipperApi, SceneId::BeforePrint);
      }
    } else {
      if (klipperApi->getExtruderTargetTempValue() == 0) {
        return new SwitchSceneRequest(klipperApi, SceneId::Printing);
      }
    }

    update_label_extruder_actual_temp();
    update_label_extruder_target_temp();

    return nullptr;
  }

  //----------------------------------------screen2----初始化------------------------------------------------------//
  void init_label_extruder_actual_temp() {
    label_ext_actual_temp = lv_label_create(lv_scr_act()); // 创建文字对象

    lv_style_set_text_font(&style_label_ext_actual_temp,
                           &font_32); // 设置字体样机及大小
    lv_style_set_text_color(&style_label_ext_actual_temp,
                            lv_color_hex(0xFF0000)); // 设置样式文本字颜色

    lv_obj_add_style(label_ext_actual_temp, &style_label_ext_actual_temp,
                     LV_PART_MAIN); // 将样式添加到文字对象中
    lv_label_set_text(label_ext_actual_temp,
                      klipperApi->getExtruderActualTemp().c_str());
    lv_obj_align(label_ext_actual_temp, LV_ALIGN_CENTER, 0, 75); // 居中显示
  }

  void init_label_extruder_target_temp() {
    label_ext_target_temp = lv_label_create(lv_scr_act()); // 创建文字对象

    lv_style_set_text_font(&style_label_ext_target_temp,
                           &font_32); // 设置字体样机及大小
    lv_style_set_text_color(&style_label_ext_target_temp,
                            lv_color_hex(0xFF0000)); // 设置样式文本字颜色

    lv_obj_add_style(label_ext_target_temp, &style_label_ext_target_temp,
                     LV_PART_MAIN); // 将样式添加到文字对象中
    lv_label_set_text(label_ext_target_temp,
                      klipperApi->getExtruderTargetTemp().c_str());
    lv_obj_align(label_ext_target_temp, LV_ALIGN_CENTER, 0, -75); // 居中显示
  }

  //-----------------------------------------------screen2--刷新-----------------------------------------------------//
  void update_label_extruder_actual_temp() {
    label_ext_actual_temp = lv_label_create(lv_scr_act()); // 创建文字对象

    lv_obj_add_style(label_ext_actual_temp, &style_label_ext_actual_temp,
                     LV_PART_MAIN); // 将样式添加到文字对象中
    lv_label_set_text(label_ext_actual_temp,
                      klipperApi->getExtruderActualTemp().c_str());
    lv_obj_align(label_ext_actual_temp, LV_ALIGN_CENTER, 0, 75); // 居中显示
  }

  void update_label_extruder_target_temp() {
    label_ext_target_temp = lv_label_create(lv_scr_act()); // 创建文字对象

    lv_obj_add_style(label_ext_target_temp, &style_label_ext_target_temp,
                     LV_PART_MAIN); // 将样式添加到文字对象中
    lv_label_set_text(label_ext_target_temp,
                      klipperApi->getExtruderTargetTemp().c_str());
    lv_obj_align(label_ext_target_temp, LV_ALIGN_CENTER, 0, -75); // 居中显示
  }
};