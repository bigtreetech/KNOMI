#pragma once
#include "KlipperApiRequest.h"

class KnomiStatusRequest : public KlipperApiRequest {
  const char *getUrl() override { return "/printer/objects/query?gcode_macro%20_KNOMI_STATUS"; }

  void processJson(JsonDocument &doc) override {

    isHoming = doc["result"]["status"]["gcode_macro _KNOMI_STATUS"]["homing"].as<bool>();
    isProbing = doc["result"]["status"]["gcode_macro _KNOMI_STATUS"]["probing"].as<bool>();
    isQgling = doc["result"]["status"]["gcode_macro _KNOMI_STATUS"]["qgling"].as<bool>();
    isHeatingNozzle = doc["result"]["status"]["gcode_macro _KNOMI_STATUS"]["heating_nozzle"].as<bool>();
    isHeatingBed = doc["result"]["status"]["gcode_macro _KNOMI_STATUS"]["heating_bed"].as<bool>();

    LV_LOG_DEBUG("Knomi status: isHoming: %d, isProbing: %d, isQgling: %d, heatingNozzle: %d, heatingBed: %d", isHoming,
                 isProbing, isQgling, isHeatingNozzle, isHeatingBed);
  }

public:
  bool isHoming = false;
  bool isProbing = false;
  bool isQgling = false;
  bool isHeatingNozzle = false;
  bool isHeatingBed = false;
};
