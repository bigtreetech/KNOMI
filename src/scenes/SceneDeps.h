#pragma once

class KlipperApi;
class WifiManager;
class KnomiWebServer;
class UIConfig;
class DisplayHAL;
class UpdateProgress;

struct SceneDeps {
public:
  KlipperApi *klipperApi = nullptr;
  UpdateProgress *progress = nullptr;
  WifiManager *mgr = nullptr;
  KnomiWebServer *webServer = nullptr;
  UIConfig *styles = nullptr;
  DisplayHAL *displayHAL = nullptr;

  SceneDeps(KlipperApi *api, UpdateProgress *progress, WifiManager *mgr, KnomiWebServer *webServer, UIConfig *styles,
            DisplayHAL *displayHAL) {
    this->klipperApi = api;
    this->progress = progress;
    this->mgr = mgr;
    this->webServer = webServer;
    this->styles = styles;
    this->displayHAL = displayHAL;
  }
};