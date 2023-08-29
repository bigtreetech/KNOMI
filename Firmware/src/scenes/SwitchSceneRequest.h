#pragma once
class AbstractScene;
class KlipperApi;

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
  Printing1Percent = 99,
  Printing100Percent = 100,
  BootupLogo = 1
};

class SwitchSceneRequest {
private:
  KlipperApi *api;

public:
  SceneId id;
  int timerOverride;

  explicit SwitchSceneRequest(KlipperApi *api, SceneId id, int timerOverride = -1) {
    this->api = api;
    this->id = id;
    this->timerOverride = timerOverride;
  }

  AbstractScene *Provide();;
};
