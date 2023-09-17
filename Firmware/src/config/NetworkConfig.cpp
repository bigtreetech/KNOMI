#include "NetworkConfig.h"

void NetworkConfig::load() {
  Preferences prefs;
  prefs.begin(this->getConfigNamespace().c_str(), true);
  this->ssid = prefs.getString("ssid");
  this->psk = prefs.getString("psk");
  this->hostname = prefs.getString("hostname", "KNOMI");
  prefs.end();
}

void NetworkConfig::save() {
  Preferences prefs;
  prefs.begin(this->getConfigNamespace().c_str(), false);
  prefs.putString("ssid", this->getSsid());
  prefs.putString("psk", this->getPsk());
  prefs.putString("hostname", this->getHostname());
  prefs.end();
}