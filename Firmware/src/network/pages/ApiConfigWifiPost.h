#pragma once
#include "AbstractPage.h"

class ApiConfigWifiPost : public AbstractPage {
  WifiManager *wifimanager = nullptr;

public:
  explicit ApiConfigWifiPost(WifiManager *manager, httpd_handle_t server)
      : AbstractPage(server, HTTP_POST, "/api/configwifi") {
    this->wifimanager = manager;
  }

  esp_err_t handler(httpd_req_t *req) override {
    /* TODO
    pServer->on("/api/configwifi", HTTP_POST, [&](AsyncWebServerRequest *req) {
 if (req->hasArg("ssid")) {
   String wifi_ssid = req->arg("ssid");
   this->httpdConfig->getNetworkConfig()->setSsid(wifi_ssid);
   LV_LOG_INFO(("got ssid:" + wifi_ssid).c_str());
 } else {
   LV_LOG_INFO("error, not found ssid");
   req->send(500, "application/json", "{error:\"SSID is not found\"}");
   return;
 }
 if (req->hasArg("pass")) {
   String wifi_pass = req->arg("pass");
   this->httpdConfig->getNetworkConfig()->setPsk(wifi_pass);
   LV_LOG_INFO(("got password:" + wifi_pass).c_str());
 } else {
   LV_LOG_INFO("error, not found password");
   req->send(500, "application/json", "{error:\"PASS is not found\"}");
   return;
 }

if (req->hasArg("hostname")) {
 String hostname = req->arg("hostname");
 this->httpdConfig->getNetworkConfig()->setHostname(hostname);
 LV_LOG_INFO(("got hostname:" + hostname).c_str());
} else {
 LV_LOG_INFO("error, not found klipper ip");
 req->send(500, "application/json", "{error:\"KLIPPER is not found\"}");
 return;
}

this->httpdConfig->getNetworkConfig()->save();

this->httpdConfig->setInitialised();
this->httpdConfig->save();

if (req->hasArg("klipper")) {
 String klipper_ip = req->arg("klipper");
 this->httpdConfig->getKlipperConfig()->setHost(klipper_ip);
 LV_LOG_INFO(("got KlipperIP:" + klipper_ip).c_str());
} else {
 LV_LOG_INFO("error, not found klipper ip");
 req->send(500, "application/json", "{error:\"KLIPPER is not found\"}");
 return;
}

this->httpdConfig->getKlipperConfig()->save();
delay(200);

req->send(200, "application/json", "{result: \"ok\"}");
LV_LOG_INFO(("WiFi Connect SSID: %s, PASS: %s, HOSTNAME: %s",
            this->httpdConfig->getNetworkConfig()->getSsid().c_str(),
this->httpdConfig->getNetworkConfig()->getPsk().c_str(), this->httpdConfig->getNetworkConfig()->getHostname().c_str()));
wifimanager->connectToWiFi();
});
    });*/
    return ESP_OK;
  }
};