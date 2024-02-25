#pragma once
#include "KlipperApiRequest.h"

class HeatingAndOverallRequest : public KlipperApiRequest {
  const char *getUrl() override { return "/api/printer"; }

  void processJson(JsonDocument &doc) override {
    String nameStr1 = doc["temperature"]["bed"]["actual"].as<String>();
    String nameStr2 = doc["temperature"]["bed"]["target"].as<String>();
    String nameStr3 = doc["temperature"]["tool0"]["actual"].as<String>();
    String nameStr4 = doc["temperature"]["tool0"]["target"].as<String>();
    String nameStr5 = doc["state"]["flags"]["printing"].as<String>();
    String nameStr6 = doc["state"]["flags"]["paused"].as<String>();

    bedtemp_actual = (uint16_t)((doc["temperature"]["bed"]["actual"].as<double>()) * 100);
    bedtemp_target = (uint16_t)((doc["temperature"]["bed"]["target"].as<double>()) * 100);
    tooltemp_actual = (uint16_t)((doc["temperature"]["tool0"]["actual"].as<double>()) * 100);
    tooltemp_target = (uint16_t)((doc["temperature"]["tool0"]["target"].as<double>()) * 100);

    text_ext_actual_temp = nameStr3 + " C"; // TODO return degree (°) sign
    text_ext_target_temp = nameStr4 + " C"; // TODO return degree (°) sign
    text_bed_actual_temp = nameStr1 + " C"; // TODO return degree (°) sign
    text_bed_target_temp = nameStr2 + " C"; // TODO return degree (°) sign

    if (nameStr5 == "true") {
      text_print_status = "Printing";
      print_status = 1;
    } else {
      if (nameStr6 == "true") {
        text_print_status = "paused";
        print_status = 2;
      } else {
        text_print_status = "standby";
        print_status = 0;
      }
    }
    LV_LOG_DEBUG("Ext: %s / %s, Bed: %s / %s, Status: %i / %s", text_ext_actual_temp.c_str(),
                 text_ext_target_temp.c_str(), text_bed_actual_temp.c_str(), text_bed_target_temp.c_str(), print_status,
                 text_print_status.c_str());
  }

public:
  String text_print_status = "standby"; // 打印状态
  uint8_t print_status;
  String text_ext_actual_temp = " C"; // TODO return degree (°) sign
  String text_ext_target_temp = " C";
  String text_bed_actual_temp = " C";
  String text_bed_target_temp = " C";
  uint16_t bedtemp_actual = 0;
  uint16_t bedtemp_target = 0;
  uint16_t tooltemp_actual = 0;
  uint16_t tooltemp_target = 0;
};
