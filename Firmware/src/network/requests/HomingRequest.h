#pragma once
#include "KlipperApiRequest.h"

class HomingRequest : public KlipperApiRequest {
  const char *getUrl() override { return "/printer/objects/query?gcode_macro%20G28"; }

  void processJson(JsonDocument &doc) override {

    String nameStr8 = doc["result"]["status"]["gcode_macro G28"]["homing"].as<String>();
    LV_LOG_INFO(nameStr8.c_str());

    if (nameStr8 == "true") {
      homing_status = 1;
    } else {
      homing_status = 0;
    }
  }

public:
  uint8_t homing_status;
};
