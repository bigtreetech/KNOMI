#pragma once
#include "AbstractPage.h"

class ApiUploadFilePost : public AbstractPage {
public:
  explicit ApiUploadFilePost(KnomiWebServer *knomiWebServer, httpd_handle_t server)
      : AbstractPage(knomiWebServer, server, HTTP_POST, "/api/uploadFile") {}

  esp_err_t handler(httpd_req_t *req) override {
    /* TODO
    pServer->on(
   "/api/uploadFile", HTTP_POST,
   [&](AsyncWebServerRequest *req) {
     AsyncWebServerResponse *pResponse = req->beginResponse(200, "text/plain", "OK");
     pResponse->addHeader("Connection", "close");
     updateInProgress = false;
     updateTotal = 0;
     updateDone = 0;
     req->send(pResponse);
   },
   [&](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len,
       bool final) {
     if (!index) {
       auto currentFile = LittleFS.open("/" + filename, "r");
       int size = currentFile.size();
       int available = LittleFS.totalBytes() - LittleFS.usedBytes() + currentFile.size();
       currentFile.close();

int minimalAvailableSize = 8192;

int uploadSize = atoi(request->getParam("size", true)->value().c_str());

if (available < minimalAvailableSize + uploadSize) {
 return request->send(400, "text/plain", "Not enough of free space");
}

updateTotal = uploadSize;
updateDone = 0;

request->_tempFile = LittleFS.open("/" + filename, "w");

updateInProgress = true;
}

// Write chunked data to the free sketch space
if (len) {
updateDone += len;
request->_tempFile.write(data, len);
}
if (final) { // if the final flag is set then this is the last frame of
          // data
request->_tempFile.flush();
request->_tempFile.close();
}
return;
   });
    });*/
    return ESP_OK;
  }
};