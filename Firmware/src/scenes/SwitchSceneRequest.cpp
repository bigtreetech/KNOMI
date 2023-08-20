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

AbstractScene *SwitchSceneRequest::Provide() {
  switch (id) {
  case SceneId::Voron:
    return new VoronScene(api);
  case SceneId::Standby:
    return new StandbyScene(api);
  case SceneId::Leveling:
    return new LevelingScene(api);
  case SceneId::Homing:
    return new HomingScene(api);
  case SceneId::BedHeating:
    return new BedHeatingScene(api);
  case SceneId::ExtruderHeating:
    return new ExtruderHeatingScene(api);
  case SceneId::BeforePrint:
    return new BeforePrintingScene(api);
  case SceneId::Printing:
    return new PrintingScene(api);
  case SceneId::AfterPrint:
    return new AfterPrintingScene(api);
  case SceneId::Printing100Percent:
    return new Printing100PercentScene(api);
  case SceneId::Printing1Percent:
    return new Printing1PercentScene(api);
  case SceneId::NoKlipper:
    return new NoKlipperScene(api);
  }
  LV_LOG_WARN((String("Unhandled scene %s") + id).c_str());
  return new StandbyScene(api);
}
