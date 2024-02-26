#pragma once
#include "../config/Config.h"
#include "log.h"
#include "requests/HeatingAndOverallRequest.h"
#include "requests/KlipperApiRequest.h"
#include "requests/KnomiStatusRequest.h"
#include "requests/PrintingRequest.h"
#include <ArduinoJson.h>
#include <queue>

class KlipperApi {
private:
  Config *config;

  String text_print_file_name = "No Printfile"; // 打印文件名

  HeatingAndOverallRequest req1;
  PrintingRequest req2;
  KnomiStatusRequest req3;

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

  bool isHoming() const { return req3.isHoming; }
  bool isLeveling() const { return req3.isProbing; }
  bool isQGLeveling() const { return req3.isQgling; }
  bool isPrinting() const { return req1.print_status == 1; }
  bool isHeatingBed() const { return req3.isHeatingBed; }
  bool isHeatingNozzle() const { return req3.isHeatingNozzle; }

  ulong getLastSuccessfullCall() const {
    ulong lastCall1 = req1.getLastSuccessfullCall();
    ulong lastCall2 = req2.getLastSuccessfullCall();
    ulong lastCall3 = req3.getLastSuccessfullCall();

    return max(max(lastCall1, lastCall2), lastCall3);
  }

  bool isKlipperNotAvailable() {
    int failCount = req1.getFailCount() + req2.getFailCount() + req3.getFailCount();
    return failCount > 3;
  }

  void refreshData() {
    if (config->isInitialised()) {
      String klipper_ip = config->getKlipperConfig()->getHost();
      klipper_ip.toLowerCase();
      req1.Execute(klipper_ip);
      req2.Execute(klipper_ip);
      req3.Execute(klipper_ip);
    }
  }
};