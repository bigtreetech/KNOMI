#pragma once
#include "../generated/images.h"
#include "../network/KlipperApi.h"
#include "../network/WifiManager.h"
#include "SceneDeps.h"
#include "SwitchSceneRequest.h"
#include "lvgl.h"

class AbstractScene {
protected:
  SceneDeps deps;

  explicit AbstractScene(SceneDeps deps) : deps(deps) {}

public:
  virtual SwitchSceneRequest *NextScene() = 0;
  virtual ~AbstractScene(){};
};
