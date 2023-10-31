#pragma once
#include "../DisplayHAL.h"
#include "../TextLabel.h"
#include "../Arc.h"
#include "../generated/images.h"
#include "../network/KlipperApi.h"
#include "../network/WifiManager.h"
#include "SwitchSceneRequest.h"
#include "log.h"

class AbstractScene {
protected:
  SceneDeps deps;

  explicit AbstractScene(SceneDeps deps) : deps(deps) { deps.displayHAL->setBackgroundColor(0x000000); }

public:
  virtual SwitchSceneRequest *NextScene() = 0;
  virtual ~AbstractScene(){};

  virtual void Tick() {}
};
