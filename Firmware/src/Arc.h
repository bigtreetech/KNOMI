#pragma once
#include "DisplayHAL.h"
#include "config/UIConfig.h"

class Arc {
  UIConfig* config;
  int progress = -1; // undefinite

public:
  Arc(UIConfig* config) {
    this->config = config;

    // arc width = 16 or 24
  }

  ~Arc() {

  }

  void setProgress(int progress) {

  }

  void tick(DisplayHAL* hal) {
    int color = config->getAccentColor();

  }
};