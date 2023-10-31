#pragma once
#include "DisplayHAL.h"
#include "config/UIConfig.h"

class TextLabel {
  UIConfig *config;
  int size;
  int xOffset;
  int yOffset;
  String text;

  int16_t text_max_width = -1;

public:
  explicit TextLabel(UIConfig *config, int size, int xOffset, int yOffset) {
    this->config = config;
    this->size = size;
    this->xOffset = xOffset;
    this->yOffset = yOffset;
  }

  void setText(String &textValue) { this->text = textValue; }

  void tick(DisplayHAL *hal) {
    uint32_t color = config->getAccentColor();
    hal->tft->startWrite();

    hal->tft->setTextColor(DisplayHAL::toSpiColor(color));
    hal->tft->setTextSize(DisplayHAL::toSpiFontSize(size));
    hal->tft->setTextDatum(MC_DATUM);

    int16_t currentTextWidth = hal->tft->textWidth(this->text.c_str());
    text_max_width = max(text_max_width, currentTextWidth);

    hal->tft->setTextPadding(max(0, text_max_width - currentTextWidth));

    int centerX = 240 + xOffset;
    int centerY = 240 + yOffset;

    hal->tft->drawString(this->text, centerX, centerY);
    hal->tft->endWrite();
  }
};