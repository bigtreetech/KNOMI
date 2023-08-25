#pragma once
#include "WifiConfig.h"
#include "lvgl.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

class KlipperApi {
private:
  uint8_t screen_no_klipper_dis_flg = 0; // 0 连接klipper失败
  uint32_t httprequest_nowtime = 0;
  uint32_t httprequest_nexttime = 0;
  WifiConfig *wifiEepromConfig;
  uint8_t start_http_request_flg = 0; // 0 开始启动http请求

  uint16_t bedtemp_actual = 0;
  uint16_t bedtemp_target = 0;
  uint16_t tooltemp_actual = 0;
  uint16_t tooltemp_target = 0;
  uint16_t last_bedtemp_target = 0;
  uint16_t last_tooltemp_target = 0;

  int httpswitch = 1;
  String nameStrpriting = "0";
  String text_print_status = "standby";         // 打印状态
  String text_print_file_name = "No Printfile"; // 打印文件名

  String text_ext_actual_temp = " °C";
  String text_ext_target_temp = " °C";
  String text_bed_actual_temp = " °C";
  String text_bed_target_temp = " °C";
  uint8_t print_status;
  int16_t progress_data = 0;
  int16_t fanspeed_data = 0;
  uint8_t homing_status;
  uint8_t levelling_status;

public:
  KlipperApi(WifiConfig *config) { wifiEepromConfig = config; }
  String getPrintStatus() { return {text_print_status}; }
  String getExtruderActualTemp() { return {text_ext_actual_temp}; }
  String getExtruderTargetTemp() { return {text_ext_target_temp}; }
  String getBedActualTemp() { return {text_bed_actual_temp}; }
  String getBedTargetTemp() { return {text_bed_target_temp}; }
  int getProgressData() const { return progress_data; }

  int getExtruderActualTempValue() const { return tooltemp_actual; }
  int getExtruderTargetTempValue() const { return tooltemp_target; }
  int getBedActualTempValue() const { return bedtemp_actual; }
  int getBedTargetTempValue() const { return bedtemp_target; }

  bool isHoming() const { return homing_status == 1; }
  bool isLeveling() const { return levelling_status == 1; }
  bool isPrinting() const { return print_status == 1; }

  bool isKlipperNotAvailable() const { return screen_no_klipper_dis_flg > 3; }

  void refreshData() {
    start_http_request_flg = 1;
  }

  void tick() {
    httprequest_nowtime = millis();
    if (httprequest_nowtime > httprequest_nexttime) {

      if (start_http_request_flg == 1) { // wifi已经连接成功，发送http请求获取数据

        HTTPClient http;

        String klipper_ip = wifiEepromConfig->getKlipperIp();
        if (httpswitch == 1) {
          http.begin("http://" + klipper_ip + "/api/printer"); // 获取温度
        } else if (httpswitch == 2) {
          http.begin("http://" + klipper_ip +
                     "/printer/objects/query?display_status"); // 获取打印
        } else if (httpswitch == 3) {
          http.begin(
              "http://" + klipper_ip +
              "/printer/objects/query?gcode_macro%20G28"); // 获取home状态
        } else if (httpswitch == 4) {
          http.begin(
              "http://" + klipper_ip +
              "/printer/objects/query?gcode_macro%20BED_MESH_CALIBRATE"); // 获取levelling状态
        } else {
        }

        int httpCode = http.GET(); // Make the request

        if (httpCode > 0) { // Check for the returning code

          screen_no_klipper_dis_flg = 0;

          String payload = http.getString();
          DynamicJsonDocument doc(payload.length() * 2);
          deserializeJson(doc, payload);

          if (httpswitch == 1) { // 喷头热床温度显示

            String nameStr1 = doc["temperature"]["bed"]["actual"].as<String>();
            String nameStr2 = doc["temperature"]["bed"]["target"].as<String>();
            String nameStr3 =
                doc["temperature"]["tool0"]["actual"].as<String>();
            String nameStr4 =
                doc["temperature"]["tool0"]["target"].as<String>();
            String nameStr5 = doc["state"]["flags"]["printing"].as<String>();
            String nameStr6 = doc["state"]["flags"]["paused"].as<String>();

            bedtemp_actual =
                (uint16_t)((doc["temperature"]["bed"]["actual"].as<double>()) *
                           100);
            bedtemp_target =
                (uint16_t)((doc["temperature"]["bed"]["target"].as<double>()) *
                           100);
            tooltemp_actual = (uint16_t)((doc["temperature"]["tool0"]["actual"]
                                              .as<double>()) *
                                         100);
            tooltemp_target = (uint16_t)((doc["temperature"]["tool0"]["target"]
                                              .as<double>()) *
                                         100);

            text_ext_actual_temp = nameStr3 + "°C";
            text_ext_target_temp = nameStr4 + "°C";
            text_bed_actual_temp = nameStr1 + "°C";
            text_bed_target_temp = nameStr2 + "°C";

            LV_LOG_INFO(text_ext_actual_temp.c_str());
            LV_LOG_INFO(text_ext_target_temp.c_str());
            LV_LOG_INFO(text_bed_actual_temp.c_str());
            LV_LOG_INFO(text_bed_target_temp.c_str());

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

            last_bedtemp_target = bedtemp_target;
            last_tooltemp_target = tooltemp_target;

            httpswitch = 2;
          } else if (httpswitch == 2) { // 打印进度

            double nameStr7 =
                (doc["result"]["status"]["display_status"]["progress"]
                     .as<double>()) *
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

            httpswitch = 3;
          } else if (httpswitch == 3) { // home状态

            String nameStr8 =
                doc["result"]["status"]["gcode_macro G28"]["homing"]
                    .as<String>();
            LV_LOG_INFO(nameStr8.c_str());

            if (nameStr8 == "true") {
              homing_status = 1;
            } else {
              homing_status = 0;
            }

            httpswitch = 4;
          } else if (httpswitch == 4) { // levelling状态

            String nameStr9 = doc["result"]["status"]
                                 ["gcode_macro BED_MESH_CALIBRATE"]["probing"]
                                     .as<String>();
            LV_LOG_INFO(nameStr9.c_str());

            if (nameStr9 == "true") {
              levelling_status = 1;
            } else {
              levelling_status = 0;
            }

            httpswitch = 1;
          } else {
          }

        } else {

          if (screen_no_klipper_dis_flg < 10)
            screen_no_klipper_dis_flg++;

          LV_LOG_INFO("Error on HTTP request");
        }
        http.end(); // Free the resources
      }

      httprequest_nexttime = httprequest_nowtime + 97UL;
    }
  }
};
