#pragma once
#include "WifiConfig.h"
#include "lvgl.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <queue>
#include "requests/KlipperApiRequest.h"
#include "requests/Request1.h"
#include "requests/Request2.h"
#include "requests/Request3.h"
#include "requests/Request4.h"

class KlipperApi {
private:
  uint8_t screen_no_klipper_dis_flg = 0; // 0 连接klipper失败
  uint32_t httprequest_nowtime = 0;
  uint32_t httprequest_nexttime = 0;
  WifiConfig *wifiEepromConfig;
  uint8_t start_http_request_flg = 0; // 0 开始启动http请求

  String text_print_file_name = "No Printfile"; // 打印文件名

  std::queue<KlipperApiRequest *> requests;
  Request1 req1;
  Request2 req2;
  Request3 req3;
  Request4 req4;

public:
  KlipperApi(WifiConfig *config) {
    wifiEepromConfig = config;
    requests.push(&req1);
    requests.push(&req2);
    requests.push(&req3);
    requests.push(&req4);
  }

  String getExtruderActualTemp() { return {req1.text_ext_actual_temp}; }
  String getExtruderTargetTemp() { return {req1.text_ext_target_temp}; }
  String getBedActualTemp() { return {req1.text_bed_actual_temp}; }
  String getBedTargetTemp() { return {req1.text_bed_target_temp}; }
  int getProgressData() const { return req2.progress_data; }

  int getExtruderActualTempValue() const { return req1.tooltemp_actual; }
  int getExtruderTargetTempValue() const { return req1.tooltemp_target; }
  int getBedActualTempValue() const { return req1.bedtemp_actual; }
  int getBedTargetTempValue() const { return req1.bedtemp_target; }

  bool isHoming() const { return req3.homing_status == 1; }
  bool isLeveling() const { return req4.levelling_status == 1; }
  bool isPrinting() const { return req1.print_status == 1; }

  bool isKlipperNotAvailable() const { return screen_no_klipper_dis_flg > 3; }

  void refreshData() { start_http_request_flg = 1; }

  void tick() {
    httprequest_nowtime = millis();
    if (httprequest_nowtime > httprequest_nexttime) {

      if (start_http_request_flg == 1) {
        KlipperApiRequest *request = requests.front();
        requests.pop();
        requests.push(request);
        String klipper_ip = wifiEepromConfig->getKlipperIp();
        if (request->Execute(klipper_ip)) {
          screen_no_klipper_dis_flg = 0;
        } else {
          if (screen_no_klipper_dis_flg < 10)
            screen_no_klipper_dis_flg++;
        }
      }

      httprequest_nexttime = httprequest_nowtime + 97UL;
    }
  }
};
