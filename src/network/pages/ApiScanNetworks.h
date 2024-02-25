#pragma once
#include "AbstractPage.h"
#include "ArduinoJson.h"
#include "network/WifiManager.h"

class ApiScanNetworks : public AbstractPage {
private:
  WifiManager *manager;

public:
  explicit ApiScanNetworks(httpd_handle_t server, WifiManager *manager)
      : AbstractPage(server, HTTP_GET, "/api/scanWifi") {
    this->manager = manager;
  }

  esp_err_t handler(httpd_req_t *req) override {
    auto foundNetworks = manager->scan();

    JsonArray array;
    for (NetworkInfo info : foundNetworks) {
      JsonObject obj;
      obj["name"] = info.networkName;
      obj["signal"] = info.signal;
      obj["isPublic"] = info.isPublic;
      array.add(obj);
    }

    String output;
    serializeJsonPretty(array, output);

    httpd_resp_send(req, output.c_str(), output.length());
    return ESP_OK;
  }
};