#include "Config.h"

void Config::load() {
  preferences.begin(this->getConfigNamespace().c_str(), true);
  //   Used to trigger first setup flow
  this->initialised = preferences.getBool("initialised", false);
  //   Used to determine if this is a first boot, we may need to migrate from
  //   legacy EEPROM config...
  bool hasKey = preferences.isKey("initialised");
  preferences.end();

  // Is this a first boot that requires migration from EEPROM config?
  if (!hasKey) {
    migrateLegacyConfig();
  } else {
    this->networkConfig = new NetworkConfig();
    this->klipperConfig = new KlipperConfig();
    this->uiConfig = new UIConfig();
  }
}

void Config::save() {
  preferences.begin(this->getConfigNamespace().c_str(), false);
  preferences.putBool("initialised", this->isInitialised());
  preferences.end();
}

void Config::reset() {
  this->initialised = false;
  nvs_flash_erase();
  nvs_flash_init();

  this->save();

  delay(100);
  ESP.restart();
}

void Config::migrateLegacyConfig() {
  auto legacyConfig = new LegacyConfig();
  String ssid = legacyConfig->getSsid();
  String psk = legacyConfig->getPsk();
  String klipperHost = legacyConfig->getKlipperHost();

  nvs_flash_erase(); // erase the NVS partition and...
  nvs_flash_init();  // initialize the NVS partition.

  delay(100);

  if (!ssid.isEmpty()) {
    auto networkConfig = new NetworkConfig(ssid, psk, "KNOMI");
    networkConfig->save();

    auto klipperConfig = new KlipperConfig(klipperHost, "");
    klipperConfig->save();

    LV_LOG_INFO("Migrated LegacyConfig rebooting...");
    this->initialised = true;
  }

  this->save();

  delay(100);
  ESP.restart();
}