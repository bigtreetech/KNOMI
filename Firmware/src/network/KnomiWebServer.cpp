//
// Created by g33k on 19.08.2023.
//

#include "KnomiWebServer.h"
#include "ArduinoJson.h"
#include "LittleFS.h"
#include "esp_ota_ops.h"

KnomiWebServer::KnomiWebServer(WifiConfig *config, WifiManager *manager) {
  auto *pServer = new AsyncWebServer(webPort);
  auto *pSocket = new AsyncWebSocket("/ws");
  pSocket->onEvent([&](AsyncWebSocket *_unused, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    switch (type) {
    case WS_EVT_CONNECT:
      LV_LOG_INFO("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      LV_LOG_INFO("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      // Here is an example on how to handle message from JS. Probably we won't ever need that.
      //AwsFrameInfo *info = (AwsFrameInfo*)arg;
      //if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        //data[len] = 0;
        //String message = (char*)data;
        // Check if the message is "getReadings"
        //if (strcmp((char*)data, "getReadings") == 0) {
        //if it is, send current sensor readings
        //String sensorReadings = getSensorReadings();
        //notifyClients(sensorReadings);
        //}
      //}
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    }
  });
  pServer->addHandler(pSocket);

  wificonfig = config;
  wifimanager = manager;

  pServer->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *pResponse =
        request->beginResponse_P(200, "text/html", KNOMI_HTML, KNOMI_HTML_SIZE);
    pResponse->addHeader("Content-Encoding", "gzip");
    request->send(pResponse);
  });

  pServer->onNotFound([&](AsyncWebServerRequest *req) {
    req->redirect("/");
    // req->send(404, "text/html", "Not found");
  });

  pServer->on("/api/status", HTTP_GET, [&](AsyncWebServerRequest *req) {
    AsyncResponseStream *response =
        req->beginResponseStream("application/json");
    DynamicJsonDocument doc(512);
    doc["hash"] = Version::getGitCommitSha1();
    doc["branch"] = Version::getGitBranch();
    doc["gitTimestamp"] = Version::getGitTimestamp();
    doc["buildTimestamp"] = Version::getBuildTimestamp();
    doc["ssid"] = wificonfig->getSSID();
    doc["pass"] = wificonfig->getPassword();
    doc["ip"] = wificonfig->getKlipperIp();
    doc["ota_partition"] = String(esp_ota_get_running_partition()->label);
    serializeJson(doc, *response);
    req->send(response);
  });

  pServer->serveStatic("/fs/", LittleFS, "/");

  pServer->on("/api/configwifi", HTTP_POST, [&](AsyncWebServerRequest *req) {
    if (req->hasArg("ssid")) {
      String wifi_ssid = req->arg("ssid");
      wificonfig->setSSID(wifi_ssid);
      LV_LOG_INFO(("got ssid:" + wifi_ssid).c_str());
    } else {
      LV_LOG_INFO("error, not found ssid");
      req->send(500, "application/json", "{error:\"SSID is not found\"}");
      return;
    }
    if (req->hasArg("pass")) {
      String wifi_pass = req->arg("pass");
      wificonfig->setPassword(wifi_pass);
      LV_LOG_INFO(("got password:" + wifi_pass).c_str());
    } else {
      LV_LOG_INFO("error, not found password");
      req->send(500, "application/json", "{error:\"PASS is not found\"}");
      return;
    }
    if (req->hasArg("klipper")) {
      String klipper_ip = req->arg("klipper");
      wificonfig->setKlipperIp(klipper_ip);
      LV_LOG_INFO(("got KlipperIP:" + klipper_ip).c_str());
    } else {
      LV_LOG_INFO("error, not found klipper ip");
      req->send(500, "application/json", "{error:\"KLIPPER is not found\"}");
      return;
    }
    delay(200);

    req->send(200, "application/json", "{result: \"ok\"}");
    LV_LOG_INFO(("WiFi Connect SSID:" + wificonfig->getSSID() + "  PASS:" + wificonfig->getPassword()).c_str());
    wifimanager->connectToWiFi();
  });

  pServer->on(
      "/update", HTTP_POST,
      [&](AsyncWebServerRequest *req) {
        AsyncWebServerResponse *pResponse = req->beginResponse(
            200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        pResponse->addHeader("Connection", "close");
        pResponse->addHeader("Access-Control-Allow-Origin", "*");
        req->send(pResponse);
        // Needs some time for Core 0 to send response
        delay(100);
        yield();
        delay(100);
        ESP.restart();
      },
      [&](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {

         if (!index) {
           if(!request->hasParam("MD5", true)) {
             return request->send(400, "text/plain", "MD5 parameter missing");
           }

           if(!Update.setMD5(request->getParam("MD5", true)->value().c_str())) {
             return request->send(400, "text/plain", "MD5 parameter invalid");
           }

           int cmd = (filename == "filesystem") ? U_SPIFFS : U_FLASH;
           if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { // Start with max available size
             Update.printError(Serial);
             return request->send(400, "text/plain", "OTA could not begin");
           }
         }

         // Write chunked data to the free sketch space
         if(len){
           if (Update.write(data, len) != len) {
             return request->send(400, "text/plain", "OTA could not begin");
           }
         }

         if (final) { // if the final flag is set then this is the last frame of data
           if (!Update.end(true)) { //true to set the size to the current progress
             Update.printError(Serial);
             return request->send(400, "text/plain", "Could not end OTA");
           }
         }else{
           return;
         }
      });

  this->socket = pSocket;
  this->server = pServer;
  LV_LOG_INFO("WebServer started!");
}
