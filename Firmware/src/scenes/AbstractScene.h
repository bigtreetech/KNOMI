#pragma once
#include "../generated/images.h"
#include "../network/KlipperApi.h"
#include "SwitchSceneRequest.h"
#include "lvgl.h"

class AbstractScene {
protected:
  KlipperApi *klipperApi = nullptr;
  explicit AbstractScene(KlipperApi *api) { this->klipperApi = api; }

public:
  virtual SwitchSceneRequest* NextScene() = 0;
  virtual ~AbstractScene() { };
};


