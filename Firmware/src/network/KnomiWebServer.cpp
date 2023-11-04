//
// Created by g33k on 19.08.2023.
//

#include "KnomiWebServer.h"
#include "ArduinoJson.h"
#include "LittleFS.h"
#include "esp_ota_ops.h"
#include <set>

KnomiWebServer::KnomiWebServer(Config *config, WifiManager *manager) {
  auto *pServer = new AsyncWebServer(webPort);
  auto *pSocket = new AsyncWebSocket("/ws");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  pSocket->onEvent([&](AsyncWebSocket *_unused, AsyncWebSocketClient *client, AwsEventType type, void *arg,
                       uint8_t *data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
      LV_LOG_INFO("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      LV_LOG_INFO("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      // Here is an example on how to handle message from JS. Probably we won't
      // ever need that.
      // AwsFrameInfo *info = (AwsFrameInfo*)arg;
      // if (info->final && info->index == 0 && info->len == len && info->opcode
      // == WS_TEXT) { data[len] = 0; String message = (char*)data;
      // Check if the message is "getReadings"
      // if (strcmp((char*)data, "getReadings") == 0) {
      // if it is, send current sensor readings
      // String sensorReadings = getSensorReadings();
      // notifyClients(sensorReadings);
      //}
      //}
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    }
  });
  pServer->addHandler(pSocket);

  this->config = config;
  wifimanager = manager;

  pServer->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *pResponse = request->beginResponse_P(200, "text/html", KNOMI_HTML, KNOMI_HTML_SIZE);
    pResponse->addHeader("Content-Encoding", "gzip");
    request->send(pResponse);
  });

  pServer->onNotFound([&](AsyncWebServerRequest *req) {
    req->redirect("/");
    // req->send(404, "text/html", "Not found");
  });

  pServer->on("/api/restart", HTTP_GET, [&](AsyncWebServerRequest *req) {
    AsyncWebServerResponse *pResponse = req->beginResponse(200, "text/plain", "OK");
    pResponse->addHeader("Connection", "close");
    req->send(pResponse);
    delay(100);
    yield();
    delay(100);
    ESP.restart();
  });

  pServer->on("/api/listFiles", HTTP_GET, [&](AsyncWebServerRequest *req) {
    AsyncResponseStream *response = req->beginResponseStream("application/json");
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

    serializeJsonPretty(doc, *response);
    req->send(response);
  });

  pServer->on("/api/uploadFile", HTTP_DELETE, [&](AsyncWebServerRequest *req) {
    String fn = req->getParam("filename")->value();
    bool result = LittleFS.remove("/" + fn);
    AsyncWebServerResponse *pResponse =
        req->beginResponse(result ? 200 : 404, "text/plain", result ? "OK" : "NOT_FOUND");
    pResponse->addHeader("Connection", "close");
    req->send(pResponse);
  });

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
      [&](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
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

  pServer->on("/api/coredump", HTTP_GET, [&](AsyncWebServerRequest *req) {
    req->send("application/binary", 0x10000, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
      const esp_partition_t *partition =
          esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, "coredump");
      if (partition != nullptr) {
        esp_partition_read(partition, index, buffer, maxLen);
        return maxLen;
      }
      return 0;
    });
  });

  pServer->on("/api/dumpHeap", HTTP_GET, [&](AsyncWebServerRequest *req) {
    heap_caps_print_heap_info((MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    AsyncWebServerResponse *pResponse = req->beginResponse(200);
    pResponse->addHeader("Connection", "close");
    req->send(pResponse);
  });

  pServer->on("/api/status", HTTP_GET, [&](AsyncWebServerRequest *req) {
    AsyncResponseStream *response = req->beginResponseStream("application/json");
    DynamicJsonDocument doc(512);
    doc["hash"] = Version::getGitCommitSha1();
    doc["branch"] = Version::getGitBranch();
    doc["gitTimestamp"] = Version::getGitTimestamp();
    doc["buildTimestamp"] = Version::getBuildTimestamp();

    if (this->config->getNetworkConfig() != nullptr) {
      doc["ssid"] = this->config->getNetworkConfig()->getSsid();
      doc["pass"] = this->config->getNetworkConfig()->getPsk();
      doc["hostname"] = this->config->getNetworkConfig()->getHostname();
    }

    if (this->config->getKlipperConfig() != nullptr) {
      doc["ip"] = this->config->getKlipperConfig()->getHost();
    }

    doc["ota_partition"] = String(esp_ota_get_running_partition()->label);
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
    auto heap = doc.createNestedObject("heap");
    heap["allocated_blocks"] = info.allocated_blocks;
    heap["free_blocks"] = info.free_blocks;
    heap["largest_free_block"] = info.largest_free_block;
    heap["minimum_free_bytes"] = info.minimum_free_bytes;
    heap["total_allocated_bytes"] = info.total_allocated_bytes;
    heap["total_blocks"] = info.total_blocks;
    heap["total_free_bytes"] = info.total_free_bytes;

    serializeJsonPretty(doc, *response);
    req->send(response);
  });

  pServer->serveStatic("/fs/", LittleFS, "/");

  pServer->on("/api/configwifi", HTTP_POST, [&](AsyncWebServerRequest *req) {
    if (req->hasArg("ssid")) {
      String wifi_ssid = req->arg("ssid");
      this->config->getNetworkConfig()->setSsid(wifi_ssid);
      LV_LOG_INFO(("got ssid:" + wifi_ssid).c_str());
    } else {
      LV_LOG_INFO("error, not found ssid");
      req->send(500, "application/json", "{error:\"SSID is not found\"}");
      return;
    }
    if (req->hasArg("pass")) {
      String wifi_pass = req->arg("pass");
      this->config->getNetworkConfig()->setPsk(wifi_pass);
      LV_LOG_INFO(("got password:" + wifi_pass).c_str());
    } else {
      LV_LOG_INFO("error, not found password");
      req->send(500, "application/json", "{error:\"PASS is not found\"}");
      return;
    }

    if (req->hasArg("hostname")) {
      String hostname = req->arg("hostname");
      this->config->getNetworkConfig()->setHostname(hostname);
      LV_LOG_INFO(("got hostname:" + hostname).c_str());
    } else {
      LV_LOG_INFO("error, not found klipper ip");
      req->send(500, "application/json", "{error:\"KLIPPER is not found\"}");
      return;
    }

    this->config->getNetworkConfig()->save();

    this->config->setInitialised();
    this->config->save();

    if (req->hasArg("klipper")) {
      String klipper_ip = req->arg("klipper");
      this->config->getKlipperConfig()->setHost(klipper_ip);
      LV_LOG_INFO(("got KlipperIP:" + klipper_ip).c_str());
    } else {
      LV_LOG_INFO("error, not found klipper ip");
      req->send(500, "application/json", "{error:\"KLIPPER is not found\"}");
      return;
    }

    this->config->getKlipperConfig()->save();
    delay(200);

    req->send(200, "application/json", "{result: \"ok\"}");
    LV_LOG_INFO(("WiFi Connect SSID: %s, PASS: %s, HOSTNAME: %s", this->config->getNetworkConfig()->getSsid().c_str(),
                 this->config->getNetworkConfig()->getPsk().c_str(),
                 this->config->getNetworkConfig()->getHostname().c_str()));
    wifimanager->connectToWiFi();
  });

  pServer->on(
      "/update", HTTP_POST,
      [&](AsyncWebServerRequest *req) {
        AsyncWebServerResponse *pResponse = req->beginResponse(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
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
      [&](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
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

  this->socket = pSocket;
  this->server = pServer;
  LV_LOG_INFO("WebServer started!");
}
