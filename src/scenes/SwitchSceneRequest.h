#pragma once
#include "SceneDeps.h"

class AbstractScene;

enum SceneId {
  Homing = 12,
  Standby = 2,
  Voron = 15,
  Leveling = 13,
  Printing = 5,
  BedHeating = 3,
  ExtruderHeating = 4,
  BeforePrint = 9,
  AfterPrint = 10,
  APConfig = 7,
  NoKlipper = 8,
  QGLeveling = 16,
  Printing1Percent = 99,
  Printing100Percent = 100,
  BootupLogo = 1,
  FirmwareUpdate = 666,
  Demo = 999
};

class SwitchSceneRequest {
private:
  SceneDeps deps;

public:
  SceneId id;
  int timerOverride;

  explicit SwitchSceneRequest(SceneDeps deps, SceneId id, int timerOverride = -1) : deps(deps) {
    this->deps = deps;
    this->id = id;
    this->timerOverride = timerOverride;
  }

  AbstractScene *Provide();
  ;
};
