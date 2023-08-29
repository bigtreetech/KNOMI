#pragma once

class Request3 : public KlipperApiRequest {
  String getUrl(String klipper_ip) {
    return "http://" + klipper_ip +
           "/printer/objects/query?gcode_macro%20G28";
  }

  void processJson(DynamicJsonDocument &doc) {

    String nameStr8 =
        doc["result"]["status"]["gcode_macro G28"]["homing"].as<String>();
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
