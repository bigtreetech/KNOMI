#pragma once
#include "BaseConfig.h"
#include "KlipperConfig.h"
#include "LegacyConfig.h"
#include "NetworkConfig.h"
#include <Preferences.h>
#include <lvgl.h>
#include <nvs_flash.h>

class Config : BaseConfig {
private:
  bool initialised;
  Preferences preferences;
  NetworkConfig *networkConfig;
  KlipperConfig *klipperConfig;

  void migrateLegacyConfig();
  void load();

public:
  Config() {
    this->configNamespace = "knomi";
    this->load();
  }
  NetworkConfig *getNetworkConfig() { return this->networkConfig; }
  KlipperConfig *getKlipperConfig() { return this->klipperConfig; }
  bool isInitailised() { return this->initialised; }
  void setInitailised(bool initialised) { this->initialised = initialised; }
  void save();
  void reset();
};