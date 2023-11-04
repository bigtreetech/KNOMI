#pragma once
#include "esp_http_server.h"

class KnomiWebServer;

class AbstractPage {
protected:
  KnomiWebServer *webServer;

public:
  AbstractPage(KnomiWebServer *knomiWebServer, httpd_handle_t server, http_method method, const char *path) {
    this->webServer = knomiWebServer;

    static const httpd_uri_t hello = {
        .uri = path,
        .method = method,
        .handler = handlerStatic,
        .user_ctx = this,
    };

    LV_LOG_INFO("Registering %s %s", http_method_str(method), path);
    httpd_register_uri_handler(server, &hello);
  }

  static esp_err_t handlerStatic(httpd_req_t *request) { return ((AbstractPage *)request->user_ctx)->handler(request); }

  virtual esp_err_t handler(httpd_req_t *request) = 0;
};
