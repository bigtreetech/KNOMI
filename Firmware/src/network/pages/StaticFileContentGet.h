#pragma once
#include "AbstractPage.h"

class StaticFileContentGet : public AbstractPage {
public:
  explicit StaticFileContentGet(KnomiWebServer *knomiWebServer, httpd_handle_t server)
      : AbstractPage(knomiWebServer, server, HTTP_GET, "/fs/*") {}

  esp_err_t handler(httpd_req_t *req) override { return ESP_OK; }
};