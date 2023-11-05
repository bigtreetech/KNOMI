#pragma once
#include "esp_http_server.h"

class KnomiWebServer;

class AbstractPage {
private:
  httpd_uri_t handlerInfo;

public:
  explicit AbstractPage(httpd_handle_t server, http_method method, const char *path) {
    handlerInfo = {
        .uri = path,
        .method = method,
        .handler = handlerStatic,
        .user_ctx = this,
    };

    LV_LOG_INFO("Registering %s %s", http_method_str(method), path);
    esp_err_t i = httpd_register_uri_handler(server, &handlerInfo);
    if (i != ESP_OK) {
      LV_LOG_WARN("Failed to register uri %s: %s", path, esp_err_to_name(i));
    }
  }

  static esp_err_t handlerStatic(httpd_req_t *request) { return ((AbstractPage *)request->user_ctx)->handler(request); }

  virtual esp_err_t handler(httpd_req_t *request) = 0;
};
