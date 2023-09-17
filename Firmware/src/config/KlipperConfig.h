#pragma once
#include "BaseConfig.h"
#include <Preferences.h>

class KlipperConfig : BaseConfig {
protected:
  void load();

private:
  String host;
  String apiKey;

public:
  KlipperConfig() {
    this->configNamespace = "klipper-config";
    this->load();
  }
  KlipperConfig(String host, String apiKey) : KlipperConfig() {
    this->host = host;
    this->apiKey = apiKey;
  }
  String getHost() { return this->host; }
  void setHost(String host) { this->host = host; }
  String getApiKey() { return this->apiKey; }
  void setApiKey(String apiKey) {
    this->apiKey = apiKey;
  } // TODO: Support setting klipper api key via UI + usage in req's
  void save();
};