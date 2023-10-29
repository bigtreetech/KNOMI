#pragma once
#include "../config/UIConfig.h"
#include "AbstractScene.h"

LV_FONT_DECLARE(font_32)
LV_FONT_DECLARE(font_48)

class Styles final {
  UIConfig *config = nullptr;
  static lv_style_t accentText32;
  static lv_style_t accentText48;
  static bool accentText32initialized;
  static bool accentText48initialized;

public:
  Styles(UIConfig *config) { this->config = config; }
  uint32_t getAccentColor() { return config->getAccentColor(); }

  lv_style_t *getAccentText32() {
    if (!accentText32initialized) {
      lv_style_init(&accentText32);
      lv_style_set_text_font(&accentText32, &font_32);
      lv_style_set_text_color(&accentText32, lv_color_hex(getAccentColor()));
      accentText32initialized = true;
    }
    return &accentText32;
  }

  lv_style_t *getAccentText48() {
    if (!accentText48initialized) {
      lv_style_init(&accentText48);
      lv_style_set_text_font(&accentText48, &font_48);
      lv_style_set_text_color(&accentText48, lv_color_hex(getAccentColor()));
      accentText48initialized = true;
    }
    return &accentText48;
  }
};