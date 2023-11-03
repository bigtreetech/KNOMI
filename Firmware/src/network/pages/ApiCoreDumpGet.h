#pragma once
#include "AbstractPage.h"

class ApiCoreDumpGet : public AbstractPage {
public:
  explicit ApiCoreDumpGet(KnomiWebServer *knomiWebServer, httpd_handle_t server)
      : AbstractPage(knomiWebServer, server, HTTP_GET, "/api/coredump") {}

  esp_err_t handler(httpd_req_t *req) override {
    /* TODO
    req->send("application/binary", 0x10000, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      const esp_partition_t *partition =
          esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, "coredump");
      if (partition != nullptr) {
        esp_partition_read(partition, index, buffer, maxLen);
        return maxLen;
      }
      return 0;
    });*/
    return ESP_OK;
  }
};