#pragma once
#include "DisplayHAL.h"
#include "config/UIConfig.h"

#include "fonts/Montserrat_Regular24pt7b.h"
#include "fonts/Montserrat_Regular32pt7b.h"

enum fontSize {
  small,
  large
};

class TextLabel {
  UIConfig *config;
  fontSize size;
  int xOffset;
  int yOffset;
  String text;

  int16_t text_max_width = -1;
  bool shouldDraw = false;

public:
  explicit TextLabel(UIConfig *config, fontSize size, int xOffset, int yOffset) {
    this->config = config;
    this->size = size;
    this->xOffset = xOffset;
    this->yOffset = yOffset;
  }

  void setText(String &textValue) {
    this->text = textValue;
    this->shouldDraw = true;
  }

  void tick(DisplayHAL *hal) {
    if (!shouldDraw)
      return;

    uint32_t color = config->getAccentColor();
    uint32_t bgColor = DisplayHAL::toSpiColor(0x000000);

    hal->tft->startWrite();

    if (size == fontSize::small) {
      hal->tft->setFreeFont(&Montserrat_Regular24pt7b);
    }
    if (size == fontSize::large) {
      hal->tft->setFreeFont(&Montserrat_Regular32pt7b);
    }

    hal->tft->setTextColor(DisplayHAL::toSpiColor(color), bgColor, true);
    hal->tft->setTextDatum(MC_DATUM);

    int16_t currentTextWidth = hal->tft->textWidth(this->text.c_str());
    text_max_width = max(text_max_width, currentTextWidth);
    int text_height = hal->tft->fontHeight();

    hal->tft->setTextPadding(max(0, text_max_width - currentTextWidth));

    int centerX = hal->tft->width() / 2 + xOffset;
    int centerY = hal->tft->height() / 2 + yOffset;

    hal->tft->fillRect(centerX - text_max_width / 2, centerY - text_height / 2, text_max_width, text_height, bgColor);

    hal->tft->drawString(this->text, centerX, centerY);
    hal->tft->endWrite();
    shouldDraw = false;
  }
};