#include "UIConfig.h"

void UIConfig::load() {
  Preferences prefs;
  prefs.begin(this->getConfigNamespace().c_str(), true);
  this->accentColor = prefs.getUInt("accentColor", 0xFF0000);
  this->backgroundColor = prefs.getUInt("backgroundColor", 0x000000);
  prefs.end();
}

void UIConfig::save() {
  Preferences prefs;
  prefs.begin(this->getConfigNamespace().c_str(), false);
  prefs.putUInt("accentColor", this->getAccentColor());
  prefs.putUInt("backgroundColor", this->getBackgroundColor());
  prefs.end();
}