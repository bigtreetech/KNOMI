#pragma once
#include "BaseConfig.h"
#include <Preferences.h>

class UIConfig : BaseConfig {
private:
  uint32_t accentColor;
  uint32_t backgroundColor;

public:
  UIConfig() {
    this->configNamespace = "ui-config";
    this->load();
  }
  uint32_t getAccentColor() { return this->accentColor; }
  void setAccentColor(uint32_t accentColor) { this->accentColor = accentColor; }

  uint32_t getBackgroundColor() { return this->backgroundColor; }
  void setBackgroundColor(uint32_t backgroundColor) { this->backgroundColor = backgroundColor; }
  void save();
  void load();
};