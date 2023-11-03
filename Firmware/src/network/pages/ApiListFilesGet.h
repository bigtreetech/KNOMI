#pragma once
#include "../../generated/images.h"
#include "AbstractPage.h"
#include "ArduinoJson.h"
#include "LittleFS.h"
#include <set>

class ApiListFilesGet : public AbstractPage {
public:
  explicit ApiListFilesGet(KnomiWebServer *knomiWebServer, httpd_handle_t server)
      : AbstractPage(knomiWebServer, server, HTTP_GET, "/api/listFiles") {}

  esp_err_t handler(httpd_req_t *req) override {
    httpd_resp_set_type(req, "application/json");

    DynamicJsonDocument doc(2048);
    doc["total"] = LittleFS.totalBytes();
    doc["used"] = LittleFS.usedBytes();
    const JsonArray &array = doc.createNestedArray("files");

    fs::File root = LittleFS.open("/");
    fs::File file = root.openNextFile();

    std::set<String> added;
    while (file) {
      const JsonObject &item = array.createNestedObject();
      added.insert(String(file.name()));
      item["name"] = String(file.name());
      item["size"] = file.size();

      file = root.openNextFile();
    }
    root.close();

    for (const char *file : KnownResourceImages::enumerateFiles()) {
      if (added.find(String(file)) == added.end()) {
        const JsonObject &item = array.createNestedObject();

        String fileName = String(file);
        item["name"] = fileName;
        item["size"] = -1;
      }
    }

    String output;
    serializeJsonPretty(doc, output);

    httpd_resp_send(req, output.c_str(), output.length());
    return ESP_OK;
  }
};