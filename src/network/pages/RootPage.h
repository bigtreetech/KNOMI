#pragma once
#include "AbstractPage.h"

class RootPage : public AbstractPage {
public:
  explicit RootPage(httpd_handle_t server) : AbstractPage(server, HTTP_GET, "/") {}

  esp_err_t handler(httpd_req_t *req) override {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    httpd_resp_send(req, KNOMI_HTML, KNOMI_HTML_SIZE);
    return ESP_OK;
  }
};