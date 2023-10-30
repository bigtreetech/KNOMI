#pragma once
#include "BaseConfig.h"
#include <Preferences.h>

class UIConfig : BaseConfig {
private:
  int32_t accentColor;

public:
  UIConfig() {
    this->configNamespace = "ui-config";
    this->load();
  }
  int32_t getAccentColor() { return this->accentColor; }
  void setAccentColor(int32_t accentColor) { this->accentColor = accentColor; }
  void save();
  void load();
};