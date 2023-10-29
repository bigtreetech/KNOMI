#pragma once
#include "BaseConfig.h"
#include <Preferences.h>

class UIConfig : BaseConfig {
private:
  uint32_t accentColor;

public:
  UIConfig() {
    this->configNamespace = "ui-config";
    this->load();
  }
  uint32_t getAccentColor() { return this->accentColor; }
  void setAccentColor(uint32_t accentColor) { this->accentColor = accentColor; }
  void save();
  void load();
};