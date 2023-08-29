//
// Created by g33k on 19.08.2023.
//

#include "KnomiWebServer.h"

KnomiWebServer::KnomiWebServer(WifiConfig* config, WifiManager* manager) {
  WebServer *pServer = new WebServer(webPort);
  wificonfig = config;
  wifimanager = manager;

  pServer->on("/", HTTP_GET, [&](){ handleRoot(); });
  pServer->on("/configwifi", HTTP_POST, [&](){ handleConfigWifi(); });

  server->onNotFound([&](){ handleNotFound(); });

  server->on("/update", HTTP_GET, [&]() {
    server->sendHeader("Content-Encoding", "gzip");
    server->send_P(200, "text/html", (const char *)ELEGANT_HTML,
                    ELEGANT_HTML_SIZE);
  });

  server->on("/update/identity", HTTP_GET, [&]() {
    server->send(200, "application/json", id);
  });

  server->on(
      "/update", HTTP_POST,
      [&]() {
        server->sendHeader("Connection", "close");
        server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        // Needs some time for Core 0 to send response
        delay(100);
        yield();
        delay(100);
        ESP.restart();
      },
      [&]() {
        // Actual OTA Download
        HTTPUpload &upload = server->upload();
        if (upload.status == UPLOAD_FILE_START) {
          // Serial output must be active to see the callback serial prints
          //            Serial.setDebugOutput(true);
          //            Serial.printf("Update Received: %s\n",
          //            upload.filename.c_str());
          if (upload.name == "filesystem") {
            if (!Update.begin(UPDATE_SIZE_UNKNOWN,
                              U_SPIFFS)) { // start with max available size
              Update.printError(Serial);
            }
          } else {
            if (!Update.begin(UPDATE_SIZE_UNKNOWN,
                              U_FLASH)) { // start with max available size
              Update.printError(Serial);
            }
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) !=
              upload.currentSize) {
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) { // true to set the size to the current
                                  // progress
            //                Serial.printf("Update Success:
            //                %u\nRebooting...\n", upload.totalSize);
          } else {
            Update.printError(Serial);
          }
          //            Serial.setDebugOutput(false);
        } else {
          //            Serial.printf("Update Failed Unexpectedly (likely broken
          //            connection): status=%d\n", upload.status);
        }
      });

  String shortSha = Version::getGitCommitSha1().substring(0, 8);
  String timestamp = Version::getBuildTimestamp();
  id = shortSha + " - " + timestamp;
  id = R"({ "id": ")" + id + R"(", "hardware": "ESP32" })";

  server->begin();

  this->server = pServer;
  LV_LOG_INFO("WebServer started!");
}

#define ROOT_HTML_OK                                                           \
  "<!DOCTYPE html><html><head><title>WIFI SET</title><meta name=\"viewport\" " \
  "content=\"width=device-width, initial-scale=1\"></head><style "             \
  "type=\"text/css\">.c,body {text-align: center}</style><body><form "         \
  "method=\"POST\" action=\"configwifi\"></label><p><span> Submission "        \
  "successful!</P><p><span> You may now close this page.</P> </form>"

void KnomiWebServer::handleRoot() {
  server->sendHeader("Content-Encoding", "gzip");
  server->send_P(200, "text/html", (const char *) KNOMI_HTML, KNOMI_HTML_SIZE);
}

void KnomiWebServer::handleConfigWifi() // 返回http状态
{
  if (server->hasArg("ssid")) // 判断是否有账号参数
  {
    LV_LOG_INFO("got ssid:");
    String wifi_ssid = server->arg("ssid"); // 获取html表单输入框name名为"ssid"的内容

    wificonfig->setSSID(wifi_ssid);
    LV_LOG_INFO(wifi_ssid.c_str());
  } else // 没有参数
  {
    LV_LOG_INFO("error, not found ssid");
    server->send(200, "text/html",
                "<meta charset='UTF-8'>error, not found ssid"); // 返回错误页面
    return;
  }
  // 密码与账号同理
  if (server->hasArg("pass")) {
    LV_LOG_INFO("got password:");
    String wifi_pass = server->arg("pass"); // 获取html表单输入框name名为"pwd"的内容

    wificonfig->setPassword(wifi_pass);
    LV_LOG_INFO(wifi_pass.c_str());
  } else {
    LV_LOG_INFO("error, not found password");
    server->send(200, "text/html",
                "<meta charset='UTF-8'>error, not found password");
    return;
  }
  // klipper ip
  if (server->hasArg("klipper")) {
    LV_LOG_INFO("got KlipperIP:");
    String klipper_ip = server->arg("klipper"); // 获取html表单输入框name名为"KlipperIP"的内容

    wificonfig->setKlipperIp(klipper_ip);

    LV_LOG_INFO(klipper_ip.c_str());
  } else {
    LV_LOG_INFO("error, not found klipper ip");
    server->send(200, "text/html",
                "<meta charset='UTF-8'>error, not found klipper ip");
    return;
  }
  delay(200);

  // server->send(200, "text/html", "<meta charset='UTF-8'>SSID：" + wifi_ssid +
  // "<br />password:" + wifi_pass + "<br />Trying to connect Trying to connect,
  // please manually close this page."); //返回保存成功页面
  server->send(200, "text/html", ROOT_HTML_OK); // 返回保存成功页面
  LV_LOG_INFO(("WiFi Connect SSID:" + wificonfig->getSSID() + "  PASS:" + wificonfig->getPassword()).c_str());
  wifimanager->connectToWiFi();
}
