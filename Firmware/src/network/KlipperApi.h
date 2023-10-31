#pragma once
#include "../config/Config.h"
#include "log.h"
#include "requests/KlipperApiRequest.h"
#include "requests/Request1.h"
#include "requests/Request2.h"
#include "requests/Request3.h"
#include "requests/Request4.h"
#include <ArduinoJson.h>
#include <AsyncHTTPRequest_Generic.hpp>
#include <queue>

class KlipperApi {
private:
  Config *config;

  String text_print_file_name = "No Printfile"; // 打印文件名

  Request1 req1;
  Request2 req2;
  Request3 req3;
  Request4 req4;

public:
  KlipperApi(Config *config) { this->config = config; }

  String &getExtruderActualTemp() { return {req1.text_ext_actual_temp}; }
  String &getExtruderTargetTemp() { return {req1.text_ext_target_temp}; }
  String &getBedActualTemp() { return {req1.text_bed_actual_temp}; }
  String &getBedTargetTemp() { return {req1.text_bed_target_temp}; }
  int getProgressData() const { return req2.progress_data; }

  int getExtruderActualTempValue() const { return req1.tooltemp_actual; }
  int getExtruderTargetTempValue() const { return req1.tooltemp_target; }
  int getBedActualTempValue() const { return req1.bedtemp_actual; }
  int getBedTargetTempValue() const { return req1.bedtemp_target; }

  bool isHoming() const { return req3.homing_status == 1; }
  bool isLeveling() const { return req4.levelling_status == 1; }
  bool isPrinting() const { return req1.print_status == 1; }

  bool isKlipperNotAvailable() {
    int failCount = req1.getFailCount() + req2.getFailCount() + req3.getFailCount() + req4.getFailCount();
    return failCount > 3;
  }

  void refreshData() {
    if (config->isInitialised()) {
      String klipper_ip = config->getKlipperConfig()->getHost();
      req1.Execute(klipper_ip);
      req2.Execute(klipper_ip);
      req3.Execute(klipper_ip);
      req4.Execute(klipper_ip);
    }
  }
};