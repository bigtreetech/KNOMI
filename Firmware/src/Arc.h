#pragma once
#include "DisplayHAL.h"
#include "config/UIConfig.h"

class Arc {
  UIConfig *config;
  int progress = 0;
  int thickness = 16;

public:
  explicit Arc(UIConfig *config) {
    this->config = config;

    // arc width = 16 or 24
  }

  void setProgress(int progress) { this->progress = progress; }

  void tick(DisplayHAL *hal) {
    uint32_t color = config->getAccentColor();
    uint32_t fgColor = hal->toSpiColor(color);
    uint32_t bgColor = hal->toSpiColor(0x000000);

    int start = 180;
    int end = ((360 * progress) / 100 + 180) % 360;
    int r = hal->tft->width() / 2;
    hal->tft->startWrite();

    hal->tft->drawArc(hal->tft->width() / 2, hal->tft->height() / 2, r, r - thickness, end, start, bgColor, bgColor,
                      true);
    hal->tft->drawArc(hal->tft->width() / 2, hal->tft->height() / 2, r, r - thickness, start, end, fgColor, bgColor,
                      true);
    hal->tft->endWrite();
  }
};