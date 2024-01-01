#pragma once
#include "../../config/UIConfig.h"
#include "AbstractPage.h"

class ApiThemeConfigPost : public AbstractPage {
  UIConfig *uiConfig;

public:
  explicit ApiThemeConfigPost(httpd_handle_t server, UIConfig *uiConfig)
      : AbstractPage(server, HTTP_POST, "/api/themeConfig") {
    this->uiConfig = uiConfig;
  }

  esp_err_t handler(httpd_req_t *req) override {
    String newAccentColor;
    String newBackgroundColor;
    if (!streamReadMultipart(req, [&](const String &formData, const String &fn) {
          if (formData.equals("accentColor")) {
            return readString(&newAccentColor);
          }
          if (formData.equals("backgroundColor")) {
            return readString(&newBackgroundColor);
          }
          return (ReadCallback) nullptr;
        })) {
      return ESP_OK;
    }

    if (newAccentColor.startsWith("#")) {
      newAccentColor = newAccentColor.substring(1);
    }
    if (newBackgroundColor.startsWith("#")) {
      newBackgroundColor = newBackgroundColor.substring(1);
    }

    if (newAccentColor != nullptr) {
      LV_LOG_INFO("Updating accentColor to: %s", newAccentColor.c_str());
      uiConfig->setAccentColor(strtol(newAccentColor.c_str(), NULL, 16));
    }
    if (newBackgroundColor != nullptr) {
      LV_LOG_INFO("Updating backgroundColor to: %s", newBackgroundColor.c_str());
      uiConfig->setBackgroundColor(strtol(newBackgroundColor.c_str(), NULL, 16));
    }

    uiConfig->save();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{result: \"ok\"}");
    return ESP_OK;
  }
};