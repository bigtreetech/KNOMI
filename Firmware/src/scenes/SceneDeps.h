#pragma once

class KlipperApi;
class WifiManager;
class KnomiWebServer;
class UIConfig;
class DisplayHAL;

struct SceneDeps {
public:
  KlipperApi *klipperApi = nullptr;
  WifiManager *mgr = nullptr;
  KnomiWebServer *webServer = nullptr;
  UIConfig *styles = nullptr;
  DisplayHAL *displayHAL = nullptr;

  SceneDeps(KlipperApi *api, WifiManager *mgr, KnomiWebServer *webServer, UIConfig *styles, DisplayHAL *displayHAL) {
    this->klipperApi = api;
    this->mgr = mgr;
    this->webServer = webServer;
    this->styles = styles;
    this->displayHAL = displayHAL;
  }
};