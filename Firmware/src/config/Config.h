#pragma once
#include "BaseConfig.h"
#include "KlipperConfig.h"
#include "LegacyConfig.h"
#include "NetworkConfig.h"
#include "UIConfig.h"
#include <Preferences.h>
#include <lvgl.h>
#include <nvs_flash.h>

class Config : BaseConfig {
private:
  bool initialised;
  Preferences preferences;
  NetworkConfig *networkConfig;
  KlipperConfig *klipperConfig;
  UIConfig *uiConfig;

  void migrateLegacyConfig();
  void load();

public:
  Config() {
    this->configNamespace = "knomi";
    this->load();
  }
  NetworkConfig *getNetworkConfig() { return this->networkConfig; }
  KlipperConfig *getKlipperConfig() { return this->klipperConfig; }
  UIConfig *getUiConfig() { return this->uiConfig; }
  bool isInitialised() { return this->initialised; }
  void setInitialised() { this->initialised = true; }
  void save();
  void reset();
};