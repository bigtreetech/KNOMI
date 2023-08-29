#pragma once
#include "../generated/images.h"
#include "../network/KlipperApi.h"
#include "../network/WifiManager.h"
#include "SwitchSceneRequest.h"
#include "lvgl.h"

class AbstractScene {
protected:
  KlipperApi *klipperApi = nullptr;
  WifiManager *mgr = nullptr;
  explicit AbstractScene(KlipperApi *api, WifiManager *mgr) {
    this->klipperApi = api;
    this->mgr = mgr;
  }

public:
  virtual SwitchSceneRequest *NextScene() = 0;
  virtual ~AbstractScene(){};
};
