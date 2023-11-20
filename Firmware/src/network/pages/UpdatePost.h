#pragma once
#include "AbstractPage.h"

class UpdatePost : public AbstractPage {
private:
  UpdateProgress *updateProgress;

public:
  explicit UpdatePost(httpd_handle_t server, UpdateProgress *progress) : AbstractPage(server, HTTP_POST, "/update") {
    this->updateProgress = progress;
  }

  esp_err_t handler(httpd_req_t *req) override {
    String errorCode = HTTPD_200;
    String errorText = "OK";

    String md5;
    String size;

    if (!streamReadMultipart(req, [&](const String &formData, const String &fn) {
          if (formData.equals("MD5")) {
            return readString(&md5);
          }

          if (formData.equals("size")) {
            return readString(&size);
          }

          if (formData.equals("file") || formData.equals("filesystem") || formData.equals("firmware")) {
            if (md5.isEmpty() || !Update.setMD5(md5.c_str())) {
              errorCode = HTTPD_400;
              errorText = "MD5 parameter missing";
              return (ReadCallback) nullptr;
            }

            updateProgress->total = size.toInt();
            updateProgress->current = 0;
            updateProgress->isInProgress = true;

            int cmd = (fn == "filesystem") ? U_SPIFFS : U_FLASH;
            if (!Update.begin(UPDATE_SIZE_UNKNOWN,
                              cmd)) { // Start with max available size
              Update.printError(Serial);
              errorCode = HTTPD_400;
              errorText = "OTA could not begin";
              return (ReadCallback) nullptr;
            }

            LV_LOG_INFO("UpdateProgress %i / %i", updateProgress->current, updateProgress->total);
            if (!updateProgress->waitForCanWrite()) {
              errorCode = HTTPD_500;
              errorText = "Internal error.";
              return (ReadCallback) nullptr;
            }
            LV_LOG_INFO("UpdateProgress check done");

            ReadCallback flashCallback = [&](const char *buf, int idx, int size) {
              if (size && errorCode == HTTPD_200) {
                updateProgress->current += size;
                if (Update.write((uint8_t *)buf + idx, size) != size) {
                  errorCode = HTTPD_400;
                  errorText = "OTA could not begin";
                }
              }
              LV_LOG_DEBUG("Written %i", size);
            };
            return flashCallback;
          }

          return (ReadCallback) nullptr;
        })) {
      return ESP_OK;
    }

    if (!Update.end(true)) { // true to set the size to the current updateProgress
      errorCode = HTTPD_400;
      errorText = "Could not end OTA";
      Update.printError(Serial);
    }

    httpd_resp_set_status(req, errorCode.c_str());
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_sendstr(req, errorText.c_str());
    updateProgress->isInProgress = false;
    updateProgress->total = 0;
    updateProgress->current = 0;

    if (errorCode == HTTPD_200) {
      delay(100);
      yield();
      delay(100);
      ESP.restart();
    }

    return ESP_OK;
  }
};