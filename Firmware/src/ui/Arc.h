#pragma once
#include "DisplayHAL.h"
#include "config/UIConfig.h"

class Arc {
  UIConfig *config;
  int progress = 0;
  int thickness = 24;

  bool cleanScreen = false;
  bool startDrawed = false;
  bool needDraw = false;

public:
  explicit Arc(UIConfig *config) {
    this->config = config;

    // arc width = 16 or 24
  }

  void setProgress(int progress) {
    int newProgress = max(0, min(100, progress));
    if (newProgress < this->progress) {
      cleanScreen = true;
      startDrawed = false;
    }
    this->progress = newProgress;
    needDraw = true;
  }

  void tick(DisplayHAL *hal) {
    if (!needDraw)
      return;

    uint32_t color = config->getAccentColor();
    uint32_t fgColor = hal->toSpiColor(color);
    uint32_t bgColor = hal->toSpiColor(0x000000);

    int start = 180;
    int end = ((360 * progress) / 100 + 180) % 360;
    int r = hal->tft->width() / 2;
    int ir = r - thickness;

    hal->tft->startWrite();
    int x = hal->tft->width() / 2;
    int y = hal->tft->height() / 2;

    if (cleanScreen) {
      hal->tft->drawArc(x, y, r, ir - 1, end, start - 1, bgColor, bgColor, true);
      hal->tft->endWrite();
      hal->tft->startWrite();
      cleanScreen = false;
    }

    // if this is not closed arc = let's draw a spots at start and end, so that arc looks rounded.
    if (end != start) {
      constexpr float deg2rad = 3.14159265359 / 180.0;

      if (!startDrawed) {
        float sx = -sinf(start * deg2rad);
        float sy = +cosf(start * deg2rad);
        sx = sx * (r + ir) / 2.0 + x;
        sy = sy * (r + ir) / 2.0 + y;
        hal->tft->drawSpot(sx, sy, (r - ir) / 2.0, fgColor,
                           fgColor); // intenionally both colors are fg colors to mix with arc
        startDrawed = true;
      }

      float ex = -sinf(end * deg2rad);
      float ey = +cosf(end * deg2rad);

      ex = ex * (r + ir) / 2.0 + x;
      ey = ey * (r + ir) / 2.0 + y;
      hal->tft->drawSpot(ex, ey, (r - ir) / 2.0, fgColor, fgColor);
    }

    hal->tft->drawArc(x, y, r, ir, start, end, fgColor, bgColor, true);

    hal->tft->endWrite();
    needDraw = false;
  }
};