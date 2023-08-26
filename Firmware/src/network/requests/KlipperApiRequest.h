#pragma once

class KlipperApiRequest {
protected:
  virtual String getUrl(String klipper_ip) = 0;
  virtual void processJson(DynamicJsonDocument &doc) = 0;

public:
  bool Execute(String klipper_ip) {
    bool result = true;
    HTTPClient http;
    http.begin(getUrl(klipper_ip)); // 获取温度
    int httpCode = http.GET();      // Make the request
    if (httpCode > 0) {

      String payload = http.getString();
      DynamicJsonDocument doc(payload.length() * 2);
      deserializeJson(doc, payload);
      processJson(doc);
    } else {

      LV_LOG_INFO("Error on HTTP request");
      result = false;
    }
    http.end(); // Free the resources
    return result;
  }
};