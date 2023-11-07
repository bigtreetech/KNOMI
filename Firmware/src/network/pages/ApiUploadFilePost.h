#pragma once
#include "AbstractPage.h"
#include "esp_rom_md5.h"
#include "../UpdateProgress.h"
#include <functional>

class ApiUploadFilePost : public AbstractPage {
  UpdateProgress* updateProgress;

public:
  explicit ApiUploadFilePost(httpd_handle_t server, UpdateProgress* progress) : AbstractPage(server, HTTP_POST, "/api/uploadFile") {
    this->updateProgress = progress;
  }

  esp_err_t handler(httpd_req_t *req) override {
    String header = getHeader(req, "Content-Type");

    if (!header.startsWith("multipart/form-data")) {
      httpd_resp_send_500(req);
      return ESP_OK;
    }

    String boundary = "--" + header.substring(header.indexOf("boundary=") + 9);
    String filename;
    String size;
    auto builder = MD5Builder();
    builder.begin();

    int errorCode = 200;
    String errorText = "OK";

    fs::File tempFile;

    streamReadMultipart(req, boundary, [&](const String &formData) {
      if (formData.equals("filename")) {
        return readString(&filename);
      }
      if (formData.equals("size")) {
        return readString(&size);
      }
      if (formData.equals("file")) {
        auto currentFile = LittleFS.open("/" + filename, "r");
        int available = LittleFS.totalBytes() - LittleFS.usedBytes() + currentFile.size();
        currentFile.close();

        int minimalAvailableSize = 8192;

        int uploadSize = size.toInt();

        if (available < minimalAvailableSize + uploadSize) {
          errorCode = 400;
          errorText = "Not enough of free space";
          return (ReadCallback) nullptr;
        }

        updateProgress->total = uploadSize;
        updateProgress->current = 0;

        tempFile = LittleFS.open("/" + filename, "w");

        updateProgress->isInProgress = true;
        LV_LOG_INFO("Upload start %s", filename.c_str());

        ReadCallback md5print = [&](const char *buf, int idx, int size) {
          updateProgress->current += size;
          tempFile.write(((uint8_t *)buf)+idx, size);
          LV_LOG_INFO("Written %i", size);

          builder.add(((uint8_t *)buf) + idx, size);
        };
        return md5print;
      }

      return (ReadCallback) nullptr;
    });
    builder.calculate();
    LV_LOG_INFO(filename.c_str());
    LV_LOG_INFO(size.c_str());
    LV_LOG_INFO(builder.toString().c_str());

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_status(req, String(errorCode).c_str());
    httpd_resp_sendstr(req, errorText.c_str());
    updateProgress->isInProgress = false;
    updateProgress->total = 0;
    updateProgress->current = 0;

    tempFile.flush();
    tempFile.close();

    return ESP_OK;
  }
};