#pragma once

class Request2 : public KlipperApiRequest {
  String getUrl(String klipper_ip) {
    return "http://" + klipper_ip + "/printer/objects/query?display_status";
  }

  void processJson(DynamicJsonDocument &doc) {
    double nameStr7 =
        (doc["result"]["status"]["display_status"]["progress"].as<double>()) *
        1000;
    uint16_t datas = (uint16_t)(nameStr7);
    uint16_t datas1 = datas % 10;

    if (datas1 > 4) {
      datas = (datas + 10) / 10;
    } else {
      datas = datas / 10;
    }

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