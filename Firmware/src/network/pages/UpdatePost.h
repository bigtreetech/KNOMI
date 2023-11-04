#pragma once
#include "AbstractPage.h"

class UpdatePost : public AbstractPage {
public:
  explicit UpdatePost(httpd_handle_t server) : AbstractPage(server, HTTP_GET, "/update") {}

  esp_err_t handler(httpd_req_t *req) override {
    /* TODO
   pServer->on(
    "/update", HTTP_POST,
    [&](AsyncWebServerRequest *req) {
      AsyncWebServerResponse *pResponse =
          req->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      pResponse->addHeader("Connection", "close");
      updateInProgress = false;
      updateTotal = 0;
      updateDone = 0;
      req->send(pResponse);
      // Needs some time for Core 0 to send response
      delay(100);
      yield();
      delay(100);
      ESP.restart();
    },
    [&](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len,
        bool final) {
      if (!index) {
        if (!request->hasParam("MD5", true)) {
          return request->send(400, "text/plain", "MD5 parameter missing");
        }

if (!Update.setMD5(request->getParam("MD5", true)->value().c_str())) {
  return request->send(400, "text/plain", "MD5 parameter invalid");
}

int cmd = (filename == "filesystem") ? U_SPIFFS : U_FLASH;
if (!Update.begin(UPDATE_SIZE_UNKNOWN,
                  cmd)) { // Start with max available size
  Update.printError(Serial);
  return request->send(400, "text/plain", "OTA could not begin");
}
}

if (!updateInProgress) {
updateInProgress = true;
updateTotal = atoi(request->getParam("size", true)->value().c_str());
updateDone = 0;
}

// Write chunked data to the free sketch space
if (len) {
updateDone += len;
if (Update.write(data, len) != len) {
  return request->send(400, "text/plain", "OTA could not begin");
}
}

if (final) {               // if the final flag is set then this is the last frame of
                         // data
if (!Update.end(true)) { // true to set the size to the current progress
  Update.printError(Serial);
  return request->send(400, "text/plain", "Could not end OTA");
}
} else {
return;
}
    });
    });*/
    return ESP_OK;
  }
};