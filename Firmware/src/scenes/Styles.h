#pragma once
#include "AbstractScene.h"

LV_FONT_DECLARE(font_32)
LV_FONT_DECLARE(font_48)

class Styles final {
  static lv_style_t redText32;
  static lv_style_t redText48;
  static bool redText32initialized;
  static bool redText48initialized;

public:
  static lv_style_t *getRedText32() {
    if (!redText32initialized) {
      lv_style_init(&redText32);
      lv_style_set_text_font(&redText32, &font_32);
      lv_style_set_text_color(&redText32, lv_color_hex(0xFF0000));
      redText32initialized = true;
    }
    return &redText32;
  }

  static lv_style_t *getRedText48() {
    if (!redText48initialized) {
      lv_style_init(&redText48);
      lv_style_set_text_font(&redText48, &font_48);
      lv_style_set_text_color(&redText48, lv_color_hex(0xFF0000));
      redText48initialized = true;
    }
    return &redText48;
  }
};