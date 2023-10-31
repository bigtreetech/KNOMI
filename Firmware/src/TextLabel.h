#pragma once
#include "DisplayHAL.h"
#include "config/UIConfig.h"

class TextLabel {
  UIConfig* config;
  int size;
  int xOffset;
  int yOffset;
  String text;

public:
  TextLabel(UIConfig* config, int size, int xOffset, int yOffset) {
    this->config = config;
    this->size = size;
    this->xOffset = xOffset;
    this->yOffset = yOffset;
  }

  ~TextLabel() {

  }

  void setText(String text) {
    this->text = text;
  }

  void tick(DisplayHAL* hal) {
    int color = config->getAccentColor();
    hal->tft->text
  }

};