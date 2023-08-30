//
// Created by g33k on 19.08.2023.
//

#include "KnomiWebServer.h"
#include "ArduinoJson.h"
#include "LittleFS.h"

KnomiWebServer::KnomiWebServer(WifiConfig *config, WifiManager *manager) {
  AsyncWebServer *pServer = new AsyncWebServer(webPort);
  wificonfig = config;
  wifimanager = manager;

  pServer->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *pResponse =
        request->beginResponse_P(200, "text/html", KNOMI_HTML, KNOMI_HTML_SIZE);
    pResponse->addHeader("Content-Encoding", "gzip");
    request->send(pResponse);
  });

  pServer->onNotFound([&](AsyncWebServerRequest *req) {
    req->send(404, "text/html", "Not found");
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
    serializeJson(doc, *response);
    req->send(response);
  });

  pServer->serveStatic("/fs/", LittleFS, "/");

  pServer->on("/configwifi", HTTP_POST, [&](AsyncWebServerRequest *req) {
    if (req->hasArg("ssid")) {
      LV_LOG_INFO("got ssid:");
      String wifi_ssid = req->arg("ssid");

      wificonfig->setSSID(wifi_ssid);
      LV_LOG_INFO(wifi_ssid.c_str());
    } else // 没有参数
    {
      LV_LOG_INFO("error, not found ssid");
      req->send(200, "text/html",
                "<meta charset='UTF-8'>error, not found ssid");
      return;
    }
    // 密码与账号同理
    if (req->hasArg("pass")) {
      LV_LOG_INFO("got password:");
      String wifi_pass = req->arg("pass");

      wificonfig->setPassword(wifi_pass);
      LV_LOG_INFO(wifi_pass.c_str());
    } else {
      LV_LOG_INFO("error, not found password");
      req->send(200, "text/html",
                "<meta charset='UTF-8'>error, not found password");
      return;
    }
    // klipper ip
    if (req->hasArg("klipper")) {
      LV_LOG_INFO("got KlipperIP:");
      String klipper_ip = req->arg("klipper");

      wificonfig->setKlipperIp(klipper_ip);

      LV_LOG_INFO(klipper_ip.c_str());
    } else {
      LV_LOG_INFO("error, not found klipper ip");
      req->send(200, "text/html",
                "<meta charset='UTF-8'>error, not found klipper ip");
      return;
    }
    delay(200);

    req->send(200, "text/html", "OK"); // 返回保存成功页面
    LV_LOG_INFO(("WiFi Connect SSID:" + wificonfig->getSSID() +
                 "  PASS:" + wificonfig->getPassword())
                    .c_str());
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

  this->server = pServer;
  LV_LOG_INFO("WebServer started!");
}
