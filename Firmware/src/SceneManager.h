#pragma once
#include "scenes/AbstractScene.h"
#include "scenes/SwitchSceneRequest.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" { // extern
// "C"表示编译生成的内部符号名使用C约定。这样在c++文件中也可以调用对应c函数
#endif
extern uint8_t wifi_ap_config_flg;        // 0 wifi配网中
#ifdef __cplusplus
}
#endif

class SceneManager {
private:
  uint8_t timer_contne = 0;
  lv_timer_t *update_timer = nullptr;
  AbstractScene* currentScene = nullptr;
  SwitchSceneRequest* switchSceneRequest = nullptr;
  KlipperApi *klipperApi = nullptr;

public:
  explicit SceneManager(KlipperApi* klipperApi) {
    this->klipperApi = klipperApi;
    switchSceneRequest = new SwitchSceneRequest(klipperApi, SceneId::Standby, 0);
  }

  void SwitchSceneIfRequired() {
    if (switchSceneRequest != nullptr) {
      delete currentScene;
      currentScene = nullptr;
      update_timer = lv_timer_create(sceneTimer, 0, this);
      lv_timer_set_repeat_count(update_timer, 1);
    }
  }

  void Timer() {
    if (timer_contne > 0)
      timer_contne--;

    if (wifi_ap_config_flg == 0 && timer_contne == 0) {
      if (currentScene != nullptr) {
        timer_contne = 5;
        switchSceneRequest = currentScene->NextScene();
        if (switchSceneRequest != nullptr && switchSceneRequest->timerOverride >= 0) {
          timer_contne = switchSceneRequest->timerOverride;
        }
      }
    }

  }

  static void sceneTimer(lv_timer_t* timer) {
    auto* that = (SceneManager*) timer->user_data;
    that->currentScene = that->switchSceneRequest->Provide();
    delete that->switchSceneRequest;
    that->switchSceneRequest = nullptr;
  }
  void SwitchScene(SceneId id, int timerOverride = -1) {
    switchSceneRequest = new SwitchSceneRequest(klipperApi, id, timerOverride);
  }
};

