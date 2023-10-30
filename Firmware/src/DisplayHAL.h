#pragma once
#include "TFT_eSPI.h"
#include "Ticker.h"

class DisplayHAL {
  TFT_eSPI *tft;

public:
  DisplayHAL() {
    tft = new TFT_eSPI(240, 240);
    tft->init();
    tft->fillScreen(0);
    tft->setRotation(0);

    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);

    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
  }

  void setBackgroundColor(uint32_t color) { tft->fillScreen(color); }
};