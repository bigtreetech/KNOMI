#pragma once
#include "AbstractPage.h"
#include "LittleFS.h"

class StaticFileContentGet : public AbstractPage {
public:
  explicit StaticFileContentGet(httpd_handle_t server) : AbstractPage(server, HTTP_GET, "/fs/*") {}

  esp_err_t handler(httpd_req_t *req) override {
    String filename = String(req->uri);
    LV_LOG_INFO("Checking for filename %s", filename.c_str());
    filename = filename.substring(filename.indexOf("fs/") + 3);
    int idx = filename.indexOf('?');
    if (idx >= 0)
      filename = filename.substring(0, idx);
    idx = filename.indexOf('#');
    if (idx >= 0)
      filename = filename.substring(0, idx);

    if (!LittleFS.exists(filename)) {
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }

    // only resources on FS are GIFs as of now
    httpd_resp_set_type(req, "image/gif");

    int CHUNK_SIZE = 0x1000;
    auto *chunk = new uint8_t[CHUNK_SIZE];
    fs::File file = LittleFS.open(filename);
    size_t count;
    while ((count = file.read(chunk, CHUNK_SIZE)) > 0) {
      httpd_resp_send_chunk(req, (char *)chunk, (int)count);
    }
    file.close();

    httpd_resp_send_chunk(req, nullptr, 0);
    return ESP_OK;
  }
};