#pragma once
#include "../Version.h"
#include "../config/Config.h"
#include "../generated/images.h"
#include "../generated/knomiWebpage.h"
#include "Arduino.h"
#include "Update.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WifiManager.h"
#include "log.h"
#include <ESPmDNS.h>
#include <esp_http_server.h>
#include <esp_ota_ops.h>
#include <set>

#include "pages/ApiConfigWifiPost.h"
#include "pages/ApiCoreDumpGet.h"
#include "pages/ApiDumpHeapGet.h"
#include "pages/ApiListFilesGet.h"
#include "pages/ApiRestartPageGet.h"
#include "pages/ApiStatusGet.h"
#include "pages/ApiUploadFileDelete.h"
#include "pages/ApiUploadFilePost.h"
#include "pages/RootPage.h"
#include "pages/StaticFileContentGet.h"
#include "pages/UpdatePost.h"
#include "pages/WebsocketLog.h"

class KnomiWebServer {
private:
  httpd_handle_t server = nullptr;
  WifiManager *manager = nullptr;

  bool started = false;
  Config *config = nullptr;

  bool updateInProgress = false;
  ulong updateTotal = 0;
  ulong updateDone = 0;

  RootPage *rootPage = nullptr;
  ApiRestartPageGet *apiRestartPageGet = nullptr;
  ApiDumpHeapGet *apiDumpHeapGet = nullptr;
  ApiListFilesGet *apiListFilesGet = nullptr;
  ApiStatusGet *apiStatusGet = nullptr;
  ApiCoreDumpGet *apiCoreDumpGet = nullptr;
  ApiUploadFileDelete *apiUploadFileDelete = nullptr;
  ApiUploadFilePost *apiUploadFilePost = nullptr;
  ApiConfigWifiPost *apiConfigWifiPost = nullptr;
  UpdatePost *updatePost = nullptr;
  StaticFileContentGet *staticFileContentGet = nullptr;

  WebsocketLog *websocketPage = nullptr;

public:
  KnomiWebServer(Config *config, WifiManager *manager) {
    this->config = config;
    this->manager = manager;
  }

  ~KnomiWebServer() {
    delete rootPage;
    delete apiRestartPageGet;
    delete apiDumpHeapGet;
    delete apiListFilesGet;
    delete apiStatusGet;
    delete apiCoreDumpGet;
    delete apiUploadFileDelete;
    delete apiUploadFilePost;
    delete apiConfigWifiPost;
    delete updatePost;
    delete staticFileContentGet;

    delete websocketPage;
    httpd_stop(server);
  }

  void registerNotFound() { httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, notFoundStaticCode); }
  static esp_err_t notFoundStaticCode(httpd_req_t *req, httpd_err_code_t error) {
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_sendstr(req, "");
    return ESP_OK;
  }

  bool isUpdateInProgress() { return updateInProgress; }

  ulong getUpdateDone() { return updateDone; }
  ulong getUpdateTotal() { return updateTotal; }

  void tick() {
    if (!this->started) {
      this->started = true;
      httpd_config_t httpdConfig = HTTPD_DEFAULT_CONFIG();
      httpdConfig.lru_purge_enable = true;
      httpdConfig.uri_match_fn = httpd_uri_match_wildcard;

      LV_LOG_INFO("Starting server on port: '%d'", httpdConfig.server_port);
      if (httpd_start(&server, &httpdConfig) == ESP_OK) {
        // Set URI handlers
        LV_LOG_INFO("Registering URI handlers");
        this->rootPage = new RootPage(this, server);
        this->apiRestartPageGet = new ApiRestartPageGet(this, server);
        this->apiDumpHeapGet = new ApiDumpHeapGet(this, server);
        this->apiListFilesGet = new ApiListFilesGet(this, server);
        this->apiStatusGet = new ApiStatusGet(this, config, server);
        this->apiCoreDumpGet = new ApiCoreDumpGet(this, server);
        this->apiUploadFileDelete = new ApiUploadFileDelete(this, server);
        this->apiUploadFilePost = new ApiUploadFilePost(this, server);
        this->apiConfigWifiPost = new ApiConfigWifiPost(this, manager, server);
        this->updatePost = new UpdatePost(this, server);
        this->staticFileContentGet = new StaticFileContentGet(this, server);

        this->websocketPage = new WebsocketLog(this, server);
        registerNotFound();
      }

      LV_LOG_INFO("WebServer started!");
    } else {
      // socket->cleanupClients();
    }


  }

  void websocketLog(const char *logString) { this->websocketPage->textAll(logString); }
};