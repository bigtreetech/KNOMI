#pragma once
#include "AbstractPage.h"

class ApiThemeConfigPost : public AbstractPage {
public:
  explicit ApiThemeConfigPost(httpd_handle_t server) : AbstractPage(server, HTTP_POST, "/api/themeConfig") {}

  esp_err_t handler(httpd_req_t *req) override {
    /* TODO
    String newAccentColor;
if (req->hasArg("accentColor")) {
 newAccentColor = req->arg("accentColor");

if (newAccentColor.startsWith("#")) {
 newAccentColor = newAccentColor.substring(1);
}
}

String newBackgroundColor;
if (req->hasArg("backgroundColor")) {
newBackgroundColor = req->arg("backgroundColor");

if (newBackgroundColor.startsWith("#")) {
 newBackgroundColor = newBackgroundColor.substring(1);
}
}

if (this->config != nullptr && this->config->getUiConfig() != nullptr) {
if (newAccentColor != nullptr) {
 LV_LOG_INFO((String("Updating accentColor to: ") + newAccentColor).c_str());
 this->config->getUiConfig()->setAccentColor(strtol(newAccentColor.c_str(), NULL, 16));
}

if (newBackgroundColor != nullptr) {
 LV_LOG_INFO((String("Updating backgroundColor to: ") + newBackgroundColor).c_str());
 this->config->getUiConfig()->setBackgroundColor(strtol(newBackgroundColor.c_str(), NULL, 16));
}

this->config->getUiConfig()->save();
req->send(200, "application/json", "{result: \"ok\"}");

delay(100);
ESP.restart();
} else {
req->send(500, "text/html", "Failed to access UI config");
}
    });*/
    return ESP_OK;
  }
};