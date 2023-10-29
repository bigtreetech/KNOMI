#pragma once

class KlipperApi;
class WifiManager;
class KnomiWebServer;
class Styles;

struct SceneDeps {
public:
  KlipperApi *klipperApi = nullptr;
  WifiManager *mgr = nullptr;
  KnomiWebServer *webServer = nullptr;
  Styles *styles = nullptr;

  SceneDeps(KlipperApi *api, WifiManager *mgr, KnomiWebServer *webServer, Styles *styles) {
    this->klipperApi = api;
    this->mgr = mgr;
    this->webServer = webServer;
    this->styles = styles;
  }
};