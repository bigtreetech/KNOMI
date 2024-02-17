#include "SwitchSceneRequest.h"
#include "APConfigScene.h"
#include "AfterPrintingScene.h"
#include "BedHeatingScene.h"
#include "BeforePrintingScene.h"
#include "BootupLogo.h"
#include "DemoScene.h"
#include "ExtruderHeatingScene.h"
#include "FirmwareUpdateScene.h"
#include "HomingScene.h"
#include "LevelingScene.h"
#include "NoKlipperScene.h"
#include "Printing100PercentScene.h"
#include "Printing1PercentScene.h"
#include "PrintingScene.h"
#include "QGLevelingScene.h"
#include "StandbyScene.h"
#include "VoronScene.h"

AbstractScene *SwitchSceneRequest::Provide() {
  switch (id) {
  case SceneId::Voron:
    return new VoronScene(deps);
  case SceneId::Leveling:
    return new LevelingScene(deps);
  case SceneId::Homing:
    return new HomingScene(deps);
  case SceneId::BedHeating:
    return new BedHeatingScene(deps);
  case SceneId::ExtruderHeating:
    return new ExtruderHeatingScene(deps);
  case SceneId::BeforePrint:
    return new BeforePrintingScene(deps);
  case SceneId::Printing:
    return new PrintingScene(deps);
  case SceneId::AfterPrint:
    return new AfterPrintingScene(deps);
  case SceneId::Printing100Percent:
    return new Printing100PercentScene(deps);
  case SceneId::Printing1Percent:
    return new Printing1PercentScene(deps);
  case SceneId::NoKlipper:
    return new NoKlipperScene(deps);
  case SceneId::APConfig:
    return new APConfigScene(deps);
  case SceneId::BootupLogo:
    return new BootupLogoScene(deps);
  case SceneId::Standby:
    return new StandbyScene(deps);
  case SceneId::FirmwareUpdate:
    return new FirmwareUpdateScene(deps);
  case SceneId::Demo:
    return new DemoScene(deps);
  case SceneId::QGLeveling:
    return new QGLevelingScene(deps);
  }
  LV_LOG_WARN((String("Unhandled scene %s") + id).c_str());
  return new StandbyScene(deps);
}
