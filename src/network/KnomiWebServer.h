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

#include "UpdateProgress.h"
#include "pages/ApiConfigWifiPost.h"
#include "pages/ApiCoreDumpGet.h"
#include "pages/ApiDumpHeapGet.h"
#include "pages/ApiListFilesGet.h"
#include "pages/ApiRestartPageGet.h"
#include "pages/ApiScanNetworks.h"
#include "pages/ApiStatusGet.h"
#include "pages/ApiThemeConfigPost.h"
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

  UpdateProgress *progress = nullptr;

  RootPage *rootPage = nullptr;
  ApiRestartPageGet *apiRestartPageGet = nullptr;
  ApiScanNetworks *apiScanNetworks = nullptr;
  ApiDumpHeapGet *apiDumpHeapGet = nullptr;
  ApiListFilesGet *apiListFilesGet = nullptr;
  ApiStatusGet *apiStatusGet = nullptr;
  ApiCoreDumpGet *apiCoreDumpGet = nullptr;
  ApiUploadFileDelete *apiUploadFileDelete = nullptr;
  ApiUploadFilePost *apiUploadFilePost = nullptr;
  ApiConfigWifiPost *apiConfigWifiPost = nullptr;
  ApiThemeConfigPost *apiThemeConfigPost = nullptr;
  UpdatePost *updatePost = nullptr;
  StaticFileContentGet *staticFileContentGet = nullptr;

  WebsocketLog *websocketPage = nullptr;

public:
  KnomiWebServer(Config *config, WifiManager *manager, UpdateProgress *progress) {
    this->config = config;
    this->manager = manager;
    this->progress = progress;
  }

  ~KnomiWebServer() {
    delete rootPage;
    delete apiRestartPageGet;
    delete apiScanNetworks;
    delete apiDumpHeapGet;
    delete apiListFilesGet;
    delete apiStatusGet;
    delete apiCoreDumpGet;
    delete apiUploadFileDelete;
    delete apiUploadFilePost;
    delete apiConfigWifiPost;
    delete apiThemeConfigPost;
    delete updatePost;
    delete staticFileContentGet;

    delete websocketPage;
    httpd_stop(server);
  }

  void registerNotFound() { httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, notFoundStaticCode); }
  static esp_err_t notFoundStaticCode(httpd_req_t *req, httpd_err_code_t error) {
    LV_LOG_INFO("Failed to find page for %s", req->uri);
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_sendstr(req, "");
    return ESP_OK;
  }

  void tick() {
    if (!this->started) {
      this->started = true;
      httpd_config_t httpdConfig = HTTPD_DEFAULT_CONFIG();
      httpdConfig.lru_purge_enable = true;
      httpdConfig.uri_match_fn = httpd_uri_match_wildcard;
      httpdConfig.stack_size = 8192;
      httpdConfig.max_uri_handlers = 32;

      LV_LOG_INFO("Starting server on port: '%d'", httpdConfig.server_port);
      if (httpd_start(&server, &httpdConfig) == ESP_OK) {
        // Set URI handlers
        LV_LOG_INFO("Registering URI handlers");
        this->rootPage = new RootPage(server);
        this->apiRestartPageGet = new ApiRestartPageGet(server);
        this->apiScanNetworks = new ApiScanNetworks(server, manager);
        this->apiDumpHeapGet = new ApiDumpHeapGet(server);
        this->apiListFilesGet = new ApiListFilesGet(server);
        this->apiStatusGet = new ApiStatusGet(config, server);
        this->apiCoreDumpGet = new ApiCoreDumpGet(server);
        this->apiThemeConfigPost = new ApiThemeConfigPost(server, config->getUiConfig());
        this->apiUploadFileDelete = new ApiUploadFileDelete(server, progress);
        this->apiUploadFilePost = new ApiUploadFilePost(server, progress);
        this->apiConfigWifiPost = new ApiConfigWifiPost(manager, server, config);
        this->updatePost = new UpdatePost(server, progress);
        this->staticFileContentGet = new StaticFileContentGet(server);

        this->websocketPage = new WebsocketLog(server);
        registerNotFound();
      }

      LV_LOG_INFO("WebServer started!");
    }
  }

  void websocketLog(const char *logString) {
    if (this->websocketPage != nullptr) {
      this->websocketPage->textAll(logString);
    }
  }
};