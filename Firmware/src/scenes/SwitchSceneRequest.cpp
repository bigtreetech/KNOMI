#include "SwitchSceneRequest.h"
#include "AfterPrintingScene.h"
#include "BedHeatingScene.h"
#include "BeforePrintingScene.h"
#include "ExtruderHeatingScene.h"
#include "HomingScene.h"
#include "LevelingScene.h"
#include "NoKlipperScene.h"
#include "Printing100PercentScene.h"
#include "Printing1PercentScene.h"
#include "PrintingScene.h"
#include "StandbyScene.h"
#include "VoronScene.h"
#include "APConfigScene.h"
#include "BootupLogo.h"

AbstractScene *SwitchSceneRequest::Provide() {
  switch (id) {
  case SceneId::Voron:
    return new VoronScene(api, mgr);
  case SceneId::Leveling:
    return new LevelingScene(api, mgr);
  case SceneId::Homing:
    return new HomingScene(api, mgr);
  case SceneId::BedHeating:
    return new BedHeatingScene(api, mgr);
  case SceneId::ExtruderHeating:
    return new ExtruderHeatingScene(api, mgr);
  case SceneId::BeforePrint:
    return new BeforePrintingScene(api, mgr);
  case SceneId::Printing:
    return new PrintingScene(api, mgr);
  case SceneId::AfterPrint:
    return new AfterPrintingScene(api, mgr);
  case SceneId::Printing100Percent:
    return new Printing100PercentScene(api, mgr);
  case SceneId::Printing1Percent:
    return new Printing1PercentScene(api, mgr);
  case SceneId::NoKlipper:
    return new NoKlipperScene(api, mgr);
  case SceneId::APConfig:
    return new APConfigScene(api, mgr);
  case BootupLogo:
    return new BootupLogoScene(api, mgr);
  case SceneId::Standby:
    return new StandbyScene(api, mgr);
  }
  LV_LOG_WARN((String("Unhandled scene %s") + id).c_str());
  return new StandbyScene(api, mgr);
}
