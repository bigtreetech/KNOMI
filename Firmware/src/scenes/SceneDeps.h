#pragma once

class KlipperApi;
class WifiManager;
class KnomiWebServer;

struct SceneDeps {
public:
  KlipperApi *klipperApi = nullptr;
  WifiManager *mgr = nullptr;
  KnomiWebServer *webServer = nullptr;

  SceneDeps(KlipperApi *api, WifiManager *mgr, KnomiWebServer *webServer) {
    this->klipperApi = api;
    this->mgr = mgr;
    this->webServer = webServer;
  }
};