#pragma once
#include "../generated/images.h"
#include "../network/KlipperApi.h"
#include "../network/WifiManager.h"
#include "SwitchSceneRequest.h"
#include "log.h"
#include "ui/Arc.h"
#include "ui/DisplayHAL.h"
#include "ui/SceneTimer.h"
#include "ui/TextLabel.h"

class AbstractScene {
protected:
  SceneDeps deps;

  explicit AbstractScene(SceneDeps deps) : deps(deps) {
    deps.displayHAL->setBackgroundColor(deps.styles->getBackgroundColor());
  }

public:
  virtual SwitchSceneRequest *NextScene() = 0;
  virtual ~AbstractScene(){};

  virtual void Tick() {}
};
