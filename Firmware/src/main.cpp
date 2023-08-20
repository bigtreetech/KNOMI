#include "button/Button.h"
#include "fs/lv_port_fs_littlefs.h"
#include "network/KnomiWebServer.h"
#include "network/WifiConfig.h"
#include "network/KlipperApi.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>
#include <Ticker.h>
#include <WiFi.h>
#include <WiFiUser.h>
#include <cstdlib>
#include <iostream>
#include <lvgl.h>
#include "SceneManager.h"

LV_FONT_DECLARE(font_20)
LV_FONT_DECLARE(font_28)
LV_FONT_DECLARE(font_32)
LV_FONT_DECLARE(font_48)

WifiConfig *wifiEepromConfig = nullptr;
Button *btn = nullptr;
KnomiWebServer *webServer = nullptr;
KlipperApi* klipperApi = nullptr;
SceneManager* sceneManager = nullptr;

uint32_t keyscan_nowtime = 0;
uint32_t keyscan_nexttime = 0;

uint32_t netcheck_nowtime = 0;
uint32_t netcheck_nexttime = 0;

Ticker timer1;

static lv_disp_draw_buf_t draw_buf;    // 定义显示器变量
static lv_color_t buf[TFT_WIDTH * 10]; // 定义刷新缓存

TFT_eSPI tft = TFT_eSPI(240, 240);


//----------------------------------------//
using namespace std;

//----------------------------------------screen5----初始化------------------------------------------------------//

ResourceImage *logo;

// 显示刷新定时器

uint8_t exist_object_screen_flg = 0;   // 0 没有存在的屏幕对象
uint8_t screen_begin_dis_flg = 0;      // 0 启动while循环
// 0 开始启动http请求
uint8_t wifi_ap_config_flg = 0;        // 0 wifi配网中

// 配置界面对象定义
lv_obj_t *label_ap_config;
// 配置界面对象样式定义
lv_style_t style_label_ap_config;

lv_obj_t *open_anim_arc;
lv_style_t style_spinner_open;
lv_style_t style_bc_spinner_open;

lv_timer_t *timer_open = NULL; // 用于延时处理开机问题：网络连接和动画
lv_timer_t *timer_open_task1 = NULL;
lv_timer_t *timer_open_task2 = NULL;
lv_timer_t *timer_open_init = NULL;
lv_timer_t *timer_project_init = NULL;

int wifi_connect_ok = 0;   // wifi 连接成功标志
int wifi_connect_fail = 0; // wifi 连接失败标志

void Open_up_animation() {
  lv_style_set_arc_color(&style_spinner_open,
                         lv_color_hex(0xFF0000));  // 设置圆弧颜色
  lv_style_set_arc_width(&style_spinner_open, 16); // 设置圆弧宽度；

  lv_style_set_arc_color(&style_bc_spinner_open,
                         lv_color_hex(0x000000)); // 设置背景圆环颜色
  lv_style_set_arc_width(&style_bc_spinner_open, 16); // 设置背景圆环宽度

  open_anim_arc = lv_spinner_create(lv_scr_act(), 1000, 45);

  lv_obj_add_style(open_anim_arc, &style_spinner_open,
                   LV_PART_INDICATOR); // 应用到圆弧部分；
  lv_obj_add_style(open_anim_arc, &style_bc_spinner_open,
                   LV_PART_MAIN); // 应用到背景圆环部分；

  lv_obj_set_size(open_anim_arc, 240, 240);
  lv_obj_align(open_anim_arc, LV_ALIGN_CENTER, 0, 0);
}

void project_task_init(lv_timer_t *timer) {

  lv_obj_del(open_anim_arc);
  if (logo) {
    delete logo;
    logo = nullptr;
  }

  sceneManager->SwitchScene(SceneId::Standby, 1);

  screen_begin_dis_flg = 1;
}

// 成功连接 初始化项目 删除动画
void open_task_suc(lv_timer_t *timer) {

  timer_project_init = lv_timer_create(project_task_init, 100, NULL);
  lv_timer_set_repeat_count(timer_project_init, 1);
}

// 初始化失败  重启
void open_task_err(lv_timer_t *timer) {
  //  ESP.restart();

  timer_project_init = lv_timer_create(project_task_init, 100, NULL);
  lv_timer_set_repeat_count(timer_project_init, 1);
}

// 提示是否成功连接
void open_task_1(lv_timer_t *timer) {

  // 成功连接wifi
  if (wifi_connect_ok == 1) {
    timer_open_task1 = lv_timer_create(open_task_suc, 4000, NULL);
    lv_timer_set_repeat_count(timer_open_task1, 1);
  }
  // 未能成功连接wifi
  if (wifi_connect_fail == 1) {
    timer_open_task2 = lv_timer_create(open_task_err, 4000, NULL);
    lv_timer_set_repeat_count(timer_open_task2, 1);
  }
}

void open_task_conv(lv_timer_t *timer) {

  // 连接wifi
  connectToWiFi(); // 连接wifi，传入的是wifi连接等待时间15s

  Open_up_animation();

  timer_open = lv_timer_create(open_task_1, 100, NULL);
  lv_timer_set_repeat_count(timer_open, 1);
}

void Open_display_init() {

  if (wifi_ap_config_flg == 1) {
    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_hex(0xFFFFFF));
    logo = KnownResourceImages::get_AP_Config_Back();
    KnownResourceImages::get_AP_Config(0, -36);
    exist_object_screen_flg = 20;
    screen_begin_dis_flg = 1;
  } else {
    lv_disp_set_bg_color(lv_disp_get_default(), lv_color_hex(0x000000));
    logo = KnownResourceImages::get_BTT_LOGO();

    timer_open_init = lv_timer_create(open_task_conv, 100, NULL);
    lv_timer_set_repeat_count(timer_open_init, 1);
  }
}


//------------------------------------------------------------------------------------------------------------//
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();                            // 使能写功能
  tft.setAddrWindow(area->x1, area->y1, w, h); // 设置填充区域
  tft.pushColors((uint16_t *)&color_p->full, w * h,
                 true); // 写入颜色缓存和缓存大小
  tft.endWrite();       // 关闭写功能

  lv_disp_flush_ready(disp); // 调用区域填充颜色函数
}

void lv_display_led_Init() {
  pinMode(16, OUTPUT);    // 旧版本
  digitalWrite(16, HIGH); // 背光默认开始

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH); // 背光默认开始
}

void lv_display_Init() {
  tft.init();         // 初始化
  tft.setRotation(0); // 屏幕旋转方向（横向）
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
}

void timer1_cb() {
  lv_tick_inc(1); /* le the GUI do its work */
  btn->KeyScan();
}

#if LV_USE_LOG
void logToSerial(const char *logLine) { Serial.print(logLine); }
#endif

__attribute__((unused)) void setup() {
  Serial.begin(115200); // 波特率
  EEPROM.begin(1024);   // 分配flash空间存储配网信息
#if LV_USE_LOG
  lv_log_register_print_cb(&logToSerial);
#endif
  delay(100);

  wifiEepromConfig = new WifiConfig();
  wifiEepromConfig->ReadConfig();

  if (wifiEepromConfig->GetApModeFlag() != '8') { // 直接进入配网
    wifi_ap_config_flg = 1;
  }

  LV_LOG_INFO("SSID:%s\r\n", wifiEepromConfig->getSSID().c_str());

  btn = new Button(wifiEepromConfig);
  lv_display_Init(); // 显示初始化
  lv_port_littlefs_init();

  exist_object_screen_flg = 0;
  screen_begin_dis_flg = 0; // 所有显示对象初始化一遍

  Open_display_init();

  lv_display_led_Init(); // 晚一点开背光

  webServer = new KnomiWebServer(wifiEepromConfig);

  timer1.attach(
      0.001, timer1_cb); // 定时0.001s，即1ms，回调函数为timer1_cb，并启动定时器

  if (wifi_ap_config_flg == 1) {
    wifiConfig(); // 开始配网功能
  }
  klipperApi = new KlipperApi(wifiEepromConfig);
}


__attribute__((unused)) void loop() {
  // lv_tick_inc(1);/* le the GUI do its work */
  lv_task_handler();

  if (WiFiClass::status() == WL_CONNECTED && !btn->isPressed()) {
    if (sceneManager == nullptr)
      sceneManager = new SceneManager(klipperApi);
    klipperApi->tick();
    if (klipperApi->isNetworkFail()) {
      sceneManager->SwitchScene(SceneId::NoKlipper, 0);
    }
  }

  sceneManager->SwitchSceneIfRequired();

  if (screen_begin_dis_flg == 1) {
    //-------------HTTP请求-----------------------//
    keyscan_nowtime = millis();
    if (keyscan_nowtime > keyscan_nexttime) {

      sceneManager->Timer();
      klipperApi->refreshData();
      keyscan_nexttime = keyscan_nowtime + 400;
    }
  }

  //----------------网络连接检查，AP热点配网------------------//
  netcheck_nowtime = millis();
  if (netcheck_nowtime > netcheck_nexttime) {

    checkConnect(true); // 检测网络连接状态，参数true表示如果断开重新连接
    checkDNS_HTTP(); // 检测客户端DNS&HTTP请求，也就是检查配网页面那部分
    webServer->tick();
    netcheck_nexttime = netcheck_nowtime + 100UL;
  }
}
