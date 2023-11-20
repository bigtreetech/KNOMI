#pragma once
#include "AbstractPage.h"

class ApiConfigWifiPost : public AbstractPage {
  WifiManager *wifimanager = nullptr;
  Config *config = nullptr;

public:
  explicit ApiConfigWifiPost(WifiManager *manager, httpd_handle_t server, Config *config)
      : AbstractPage(server, HTTP_POST, "/api/configwifi") {
    this->wifimanager = manager;
    this->config = config;
  }

  esp_err_t handler(httpd_req_t *req) override {
    String ssid;
    bool setSsid = false;

    String pass;
    bool setPass = false;

    String hostname;
    bool setHostname = false;

    String klipper;
    bool setKlipper = false;

    if (!streamReadMultipart(req, [&](const String &formData, const String &fn) {
          if (formData.equals("ssid")) {
            setSsid = true;
            return readString(&ssid);
          }
          if (formData.equals("pass")) {
            setPass = true;
            return readString(&pass);
          }
          if (formData.equals("hostname")) {
            setHostname = true;
            return readString(&hostname);
          }
          if (formData.equals("klipper")) {
            setKlipper = true;
            return readString(&klipper);
          }
          return (ReadCallback) nullptr;
        })) {
      return ESP_OK;
    }

    if (setSsid) {
      this->config->getNetworkConfig()->setSsid(ssid);
      LV_LOG_INFO("got ssid: %s", ssid.c_str());
    } else {
      LV_LOG_INFO("error, not found ssid");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "{error:\"SSID is not found\"}");
      return ESP_OK;
    }

    if (setPass) {
      this->config->getNetworkConfig()->setPsk(pass);
      LV_LOG_INFO("got password: %s", pass.c_str());
    } else {
      LV_LOG_INFO("error, not found password");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "{error:\"PASS is not found\"}");
      return ESP_OK;
    }

    if (setHostname) {
      this->config->getNetworkConfig()->setHostname(hostname);
      LV_LOG_INFO("got hostname: %s", hostname.c_str());
    } else {
      LV_LOG_INFO("error, not found hostname");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "{error:\"HOSTNAME is not found\"}");
      return ESP_OK;
    }

    this->config->getNetworkConfig()->save();

    this->config->setInitialised();
    this->config->save();

    if (setKlipper) {
      this->config->getKlipperConfig()->setHost(klipper);
      LV_LOG_INFO("got KlipperIP: %s", klipper.c_str());
    } else {
      LV_LOG_INFO("error, not found klipper ip");
      httpd_resp_set_type(req, "application/json");
      httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "{error:\"KLIPPER is not found\"}");
      return ESP_OK;
    }

    this->config->getKlipperConfig()->save();
    delay(200);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{result: \"ok\"}");
    LV_LOG_INFO("WiFi Connect SSID: %s, PASS: %s, HOSTNAME: %s", this->config->getNetworkConfig()->getSsid().c_str(),
                this->config->getNetworkConfig()->getPsk().c_str(),
                this->config->getNetworkConfig()->getHostname().c_str());
    wifimanager->connectToWiFi();

    return ESP_OK;
  }
};