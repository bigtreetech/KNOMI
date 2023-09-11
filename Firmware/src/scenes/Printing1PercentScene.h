#pragma once
#include "AbstractScene.h"

LV_FONT_DECLARE(font_48);

class Printing1PercentScene : public AbstractScene {
private:
  lv_obj_t *label_print_progress;
  lv_obj_t *arc_print_progress;

  lv_style_t style_label_print_progress;
  lv_style_t style_arc_print_progress;

public:
  explicit Printing1PercentScene(KlipperApi *api, WifiManager *mgr)
      : AbstractScene(api, mgr) {
    init_label_print_progress();
    init_arc_print_progress();
  }
  ~Printing1PercentScene() override {
    lv_obj_del(label_print_progress);
    lv_obj_del(arc_print_progress);
  }

  SwitchSceneRequest *NextScene() override {
    if (klipperApi->isPrinting()) {
      if (klipperApi->getProgressData() == 100) {
        return new SwitchSceneRequest(klipperApi, mgr,
                                      SceneId::Printing100Percent, 7);
      }
    } else {
      return new SwitchSceneRequest(klipperApi, mgr, SceneId::Standby);
    }
    String result = String(klipperApi->getProgressData()) + "%";
    lv_label_set_text(label_print_progress, result.c_str());
    lv_arc_set_value(arc_print_progress, klipperApi->getProgressData());
    return nullptr;
  }

  void init_label_print_progress() {

    label_print_progress = lv_label_create(lv_scr_act()); // 创建文字对象

    lv_style_init(&style_label_print_progress);
    lv_style_set_text_font(&style_label_print_progress,
                           &font_48); // 设置字体样机及大小
    lv_style_set_text_color(&style_label_print_progress,
                            lv_color_hex(0xFF0000)); // 设置样式文本字颜色

    lv_obj_add_style(label_print_progress, &style_label_print_progress,
                     LV_PART_MAIN); // 将样式添加到文字对象中
    lv_label_set_text(label_print_progress,
                      String(klipperApi->getProgressData()).c_str());
    lv_obj_align(label_print_progress, LV_ALIGN_CENTER, 0, 0); // 居中显示
  }

  void init_arc_print_progress() {
    arc_print_progress = lv_arc_create(lv_scr_act()); // 创建圆弧对象

    lv_style_init(&style_arc_print_progress);
    lv_style_set_arc_width(&style_arc_print_progress, 24); // 设置样式的圆弧粗细
    lv_style_set_arc_color(&style_arc_print_progress,
                           lv_color_hex(0x000000)); // 设置背景圆环颜色

    lv_obj_add_style(arc_print_progress, &style_arc_print_progress,
                     LV_PART_MAIN); // 将样式应用到圆弧背景
    lv_obj_add_style(arc_print_progress, &style_arc_print_progress,
                     LV_PART_INDICATOR); // 将样式应用到圆弧前景

    lv_obj_remove_style(arc_print_progress, NULL, LV_PART_KNOB); // 移除样式
    lv_obj_clear_flag(arc_print_progress, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(arc_print_progress, lv_color_hex(0xFF0000),
                               LV_PART_INDICATOR);    // 进度条颜色
    lv_obj_set_size(arc_print_progress, 240, 240);    // 设置尺寸
    lv_arc_set_rotation(arc_print_progress, 270);     // 设置零度位置
    lv_arc_set_bg_angles(arc_print_progress, 0, 360); // 设置角度
    lv_arc_set_value(arc_print_progress, 0);          // 设置初始值
    lv_obj_center(arc_print_progress);                // 居中显示
  }
};
