#include "KlipperConfig.h"

void KlipperConfig::load() {
  Preferences prefs;
  prefs.begin(this->getConfigNamespace().c_str(), true);
  this->host = prefs.getString("host");
  this->apiKey = prefs.getString("apiKey");
  prefs.end();
}

void KlipperConfig::save() {
  Preferences prefs;
  prefs.begin(this->getConfigNamespace().c_str(), false);
  prefs.putString("host", this->getHost());
  prefs.putString("apiKey", this->getApiKey());
  prefs.end();
}