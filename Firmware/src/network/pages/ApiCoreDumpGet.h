#pragma once
#include "AbstractPage.h"

class ApiCoreDumpGet : public AbstractPage {
public:
  explicit ApiCoreDumpGet(httpd_handle_t server) : AbstractPage(server, HTTP_GET, "/api/coredump") {}

  esp_err_t handler(httpd_req_t *req) override {
    httpd_resp_set_type(req, "application/binary");

    const esp_partition_t *partition =
        esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, "coredump");

    int CHUNK_SIZE = 0x1000;
    char *chunk = new char[CHUNK_SIZE];
    for (int i = 0; i < 0x10000; i += CHUNK_SIZE) {
      esp_partition_read(partition, i, chunk, CHUNK_SIZE);
      httpd_resp_send_chunk(req, chunk, CHUNK_SIZE);
    }
    delete[] chunk;
    httpd_resp_send_chunk(req, nullptr, 0);

    return ESP_OK;
  }
};