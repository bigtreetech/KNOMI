#pragma once

class Request4 : public KlipperApiRequest {

  String getUrl(String klipper_ip) {
    return "http://" + klipper_ip +
           "/printer/objects/query?gcode_macro%20BED_MESH_CALIBRATE";
  }

  void processJson(DynamicJsonDocument &doc) {
    String nameStr9 =
        doc["result"]["status"]["gcode_macro BED_MESH_CALIBRATE"]["probing"]
            .as<String>();
    LV_LOG_INFO(nameStr9.c_str());

    if (nameStr9 == "true") {
      levelling_status = 1;
    } else {
      levelling_status = 0;
    }
  }

public:
  uint8_t levelling_status;
};