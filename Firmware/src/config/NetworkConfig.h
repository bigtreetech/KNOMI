#pragma once
#include "BaseConfig.h"
#include <Preferences.h>

class NetworkConfig : BaseConfig {
private:
  String ssid;
  String psk;
  String hostname;

public:
  NetworkConfig() {
    this->configNamespace = "network-config";
    this->load();
  }

  //   Used during migration
  NetworkConfig(String ssid, String psk, String hostname) {
    this->configNamespace = "network-config";
    this->ssid = ssid;
    this->psk = psk;
    this->hostname = hostname;
    this->save();
  }

  String getSsid() { return this->ssid; }
  void setSsid(String ssid) { this->ssid = ssid; }
  String getPsk() { return this->psk; }
  void setPsk(String psk) { this->psk = psk; }
  String getHostname() { return this->hostname; }
  void setHostname(String hostname) { this->hostname = hostname; }
  void save();
  void load();
};