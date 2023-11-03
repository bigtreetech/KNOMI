#pragma once
#include "AbstractPage.h"

class ApiRestartPageGet : public AbstractPage {
public:
  explicit ApiRestartPageGet(KnomiWebServer *knomiWebServer, httpd_handle_t server)
      : AbstractPage(knomiWebServer, server, HTTP_GET, "/api/restart") {}

  esp_err_t handler(httpd_req_t *req) override {
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, "OK");

    delay(100);
    yield();
    delay(100);
    ESP.restart();
    return ESP_OK;
  }
};