#pragma once
#include "AbstractPage.h"

class ApiUploadFileDelete : public AbstractPage {
public:
  explicit ApiUploadFileDelete(KnomiWebServer *knomiWebServer, httpd_handle_t server)
      : AbstractPage(knomiWebServer, server, HTTP_DELETE, "/api/uploadFile") {}

  esp_err_t handler(httpd_req_t *req) override {
    /* TODO
    String fn = req->getParam("filename")->value();
    bool result = LittleFS.remove("/" + fn);
    httpd_resp_set_type(req, "text/plain");
    if (result)
      httpd_resp_sendstr(req, "OK");
    else
      httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "NOT_FOUND");
    */
    return ESP_OK;
  }
};