#pragma once
#include "KlipperApiRequest.h"

class PrintingRequest : public KlipperApiRequest {
  const char* getUrl() override { return "/printer/objects/query?display_status"; }

  void processJson(JsonDocument &doc) override {
    double nameStr7 = (doc["result"]["status"]["display_status"]["progress"].as<double>()) * 100;
    uint16_t datas = round(nameStr7);

    progress_data = datas;
    if (datas == 0) {
      nameStrpriting = "0";
    } else {
      String result;
      result = String(datas);
      nameStrpriting = result;
    }
    LV_LOG_INFO(nameStrpriting.c_str());
  }

public:
  String nameStrpriting = "0";
  int16_t progress_data = 0;
};