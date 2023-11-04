#pragma once
#include "AbstractScene.h"

class ExtruderHeatingScene : public AbstractScene {
private:
  ResourceImage *ri_ext;
  TextLabel *actualTemp;
  TextLabel *targetTemp;

public:
  explicit ExtruderHeatingScene(SceneDeps deps) : AbstractScene(deps) {
    ri_ext = KnownResourceImages::get_ext_temp();
    actualTemp = new TextLabel(deps.styles, fontSize::small, 0, 75);
    targetTemp = new TextLabel(deps.styles, fontSize::small, 0, -75);
  }

  ~ExtruderHeatingScene() override {
    delete ri_ext;
    delete actualTemp;
    delete targetTemp;
  }

  SwitchSceneRequest *NextScene() override {
    if ((deps.klipperApi->getExtruderActualTempValue() >= deps.klipperApi->getExtruderTargetTempValue()) &&
        (deps.klipperApi->getExtruderTargetTempValue() != 0)) {
      // heated fully...

      if (!deps.klipperApi->isPrinting()) {
        return new SwitchSceneRequest(deps, SceneId::Standby);
      } else {
        return new SwitchSceneRequest(deps, SceneId::BeforePrint);
      }
    } else {
      if (deps.klipperApi->getExtruderTargetTempValue() == 0) {
        return new SwitchSceneRequest(deps, SceneId::Printing);
      }
    }

    return nullptr;
  }

  void Tick() override {
    actualTemp->setText(deps.klipperApi->getExtruderActualTemp());
    targetTemp->setText(deps.klipperApi->getExtruderTargetTemp());

    ri_ext->tick(deps.displayHAL);
    actualTemp->tick(deps.displayHAL);
    targetTemp->tick(deps.displayHAL);
  }
};