#pragma once
#include "../DisplayHAL.h"
#include "../generated/images.h"
#include "../network/KlipperApi.h"
#include "../network/WifiManager.h"
#include "SwitchSceneRequest.h"
#include "log.h"

class AbstractScene {
protected:
  SceneDeps deps;

  explicit AbstractScene(SceneDeps deps) : deps(deps) {}

public:
  virtual SwitchSceneRequest *NextScene() = 0;
  virtual ~AbstractScene(){};

  virtual void Tick() {}
};
