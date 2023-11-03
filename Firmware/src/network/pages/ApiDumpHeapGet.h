#pragma once
#include "AbstractPage.h"

class ApiDumpHeapGet : public AbstractPage {
public:
  explicit ApiDumpHeapGet(KnomiWebServer *knomiWebServer, httpd_handle_t server)
      : AbstractPage(knomiWebServer, server, HTTP_GET, "/api/dumpHeap") {}

  esp_err_t handler(httpd_req_t *req) override {
    heap_caps_print_heap_info((MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
  }
};