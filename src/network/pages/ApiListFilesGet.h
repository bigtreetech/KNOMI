#pragma once
#include "../../generated/images.h"
#include "AbstractPage.h"
#include "ArduinoJson.h"
#include "LittleFS.h"
#include <set>

class ApiListFilesGet : public AbstractPage {
public:
  explicit ApiListFilesGet(httpd_handle_t server) : AbstractPage(server, HTTP_GET, "/api/listFiles") {}

  esp_err_t handler(httpd_req_t *req) override {
    httpd_resp_set_type(req, "application/json");

    JsonDocument doc;
    doc["total"] = LittleFS.totalBytes();
    doc["used"] = LittleFS.usedBytes();
    const JsonArray &array = doc["files"].to<JsonArray>();

    fs::File root = LittleFS.open("/");
    fs::File file = root.openNextFile();

    std::set<String> added;
    while (file) {
      const JsonObject &item = array.add<JsonObject>();
      added.insert(String(file.name()));
      item["name"] = String(file.name());
      item["size"] = file.size();

      file = root.openNextFile();
    }
    root.close();

    for (const char *file : KnownResourceImages::enumerateFiles()) {
      if (added.find(String(file)) == added.end()) {
        const JsonObject &item = array.add<JsonObject>();

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