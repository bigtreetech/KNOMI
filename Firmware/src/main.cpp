#include "generated/images.h"
#include "lv_port_fs_littlefs.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <Ticker.h>
#include <WiFi.h>
#include <WiFiUser.h>
#include <iostream>
#include <key.h>
#include <lvgl.h>
#include <lvgl_gui.h>
#include <stdlib.h>

LV_FONT_DECLARE(font_20)
LV_FONT_DECLARE(font_28)
LV_FONT_DECLARE(font_32)
LV_FONT_DECLARE(font_48)

uint16_t bedtemp_actual = 0;
uint16_t bedtemp_target = 0;
uint16_t last_bedtemp_target = 0;
uint16_t tooltemp_actual = 0;
uint16_t tooltemp_target = 0;
uint16_t last_tooltemp_target = 0;

String text_print_status = "standby";         // 打印状态
String text_print_file_name = "No Printfile"; // 打印文件名

String text_ext_actual_temp = " °C";
String text_ext_target_temp = " °C";
String text_bed_actual_temp = " °C";
String text_bed_target_temp = " °C";

int httpswitch = 1;

String nameStrpriting = "0";

uint32_t keyscan_nowtime = 0;
uint32_t keyscan_nexttime = 0;

uint32_t netcheck_nowtime = 0;
uint32_t netcheck_nexttime = 0;

uint32_t httprequest_nowtime = 0;
uint32_t httprequest_nexttime = 0;

Ticker timer1;

static lv_disp_draw_buf_t draw_buf;    // 定义显示器变量
static lv_color_t buf[TFT_WIDTH * 10]; // 定义刷新缓存

TFT_eSPI tft = TFT_eSPI(240, 240);

int16_t progress_data = 0;
int16_t fanspeed_data = 0;

ResourceImage *ri_disconnect, *ri_standby, *ri_voron, *ri_print,
    *ri_bedTemp, *ri_extTemp, *ri_before, *ri_after, *ri_ok, *ri_home,
    *ri_level;

//----------------------------------------//
using namespace std;

//--------------------------------------screen1---初始化----------------------------------------------//
void init_label_print_status() {
  label_print_status = lv_label_create(lv_scr_act());

  lv_label_set_text(label_print_status, text_print_status.c_str());
  lv_obj_align(label_print_status, LV_ALIGN_CENTER, 0, 50); // 居中显示
}

void init_label_print_progress() {
  String result;
  result = String(progress_data);
  String TEXT = result;

  label_print_progress = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_print_progress,
                         &font_48); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_print_progress,
                          lv_color_hex(0xFF0000)); // 设置样式文本字颜色

  lv_obj_add_style(label_print_progress, &style_label_print_progress,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_print_progress, TEXT.c_str());
  lv_obj_align(label_print_progress, LV_ALIGN_CENTER, 0, 0); // 居中显示
}

void init_arc_print_progress() {
  arc_print_progress = lv_arc_create(lv_scr_act()); // 创建圆弧对象

  lv_style_set_arc_width(&style_arc_print_progress, 24); // 设置样式的圆弧粗细
  lv_style_set_arc_color(&style_arc_print_progress,
                         lv_color_hex(0x000000)); // 设置背景圆环颜色

  lv_obj_add_style(arc_print_progress, &style_arc_print_progress,
                   LV_PART_MAIN); // 将样式应用到圆弧背景
  lv_obj_add_style(arc_print_progress, &style_arc_print_progress,
                   LV_PART_INDICATOR); // 将样式应用到圆弧前景

  lv_obj_remove_style(arc_print_progress, NULL, LV_PART_KNOB); // 移除样式
  lv_obj_clear_flag(arc_print_progress, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_color(arc_print_progress, lv_color_hex(0xFF0000),
                             LV_PART_INDICATOR);    // 进度条颜色
  lv_obj_set_size(arc_print_progress, 240, 240);    // 设置尺寸
  lv_arc_set_rotation(arc_print_progress, 270);     // 设置零度位置
  lv_arc_set_bg_angles(arc_print_progress, 0, 360); // 设置角度
  lv_arc_set_value(arc_print_progress, 0);          // 设置初始值
  lv_obj_center(arc_print_progress);                // 居中显示
}

//----------------------------------------screen2----初始化------------------------------------------------------//
void init_label_extruder_actual_temp() {
  label_ext_actual_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_ext_actual_temp,
                         &font_32); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_ext_actual_temp,
                          lv_color_hex(0xFF0000)); // 设置样式文本字颜色

  lv_obj_add_style(label_ext_actual_temp, &style_label_ext_actual_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_ext_actual_temp, text_ext_actual_temp.c_str());
  lv_obj_align(label_ext_actual_temp, LV_ALIGN_CENTER, 0, 75); // 居中显示
}

void init_label_extruder_target_temp() {
  label_ext_target_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_ext_target_temp,
                         &font_32); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_ext_target_temp,
                          lv_color_hex(0xFF0000)); // 设置样式文本字颜色

  lv_obj_add_style(label_ext_target_temp, &style_label_ext_target_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_ext_target_temp, text_ext_target_temp.c_str());
  lv_obj_align(label_ext_target_temp, LV_ALIGN_CENTER, 0, -75); // 居中显示
}

void init_label_heaterbed_actual_temp() {
  label_bed_actual_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_bed_actual_temp,
                         &font_32); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_bed_actual_temp,
                          lv_color_hex(0xFF0000)); // 设置样式文本字颜色

  lv_obj_add_style(label_bed_actual_temp, &style_label_bed_actual_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_bed_actual_temp, text_bed_actual_temp.c_str());
  lv_obj_align(label_bed_actual_temp, LV_ALIGN_CENTER, 0, 75); // 居中显示
}

void init_label_heaterbed_target_temp() {
  label_bed_target_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_bed_target_temp,
                         &font_32); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_bed_target_temp,
                          lv_color_hex(0xFF0000)); // 设置样式文本字颜色

  lv_obj_add_style(label_bed_target_temp, &style_label_bed_target_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_bed_target_temp, text_bed_target_temp.c_str());
  lv_obj_align(label_bed_target_temp, LV_ALIGN_CENTER, 0, -75); // 居中显示
}

//----------------------------------------screen3----初始化------------------------------------------------------//
void init_label_print_file() {
  label_print_file = lv_label_create(lv_scr_act()); // 创建文字对象

  // 设置背景圆角半径为: 5
  lv_style_set_radius(&style_label_print_file, 3);
  // 设置背景透明度
  lv_style_set_bg_opa(&style_label_print_file, LV_OPA_COVER);
  // 设置背景颜色
  lv_style_set_bg_color(&style_label_print_file,
                        lv_palette_lighten(LV_PALETTE_BLUE, 1));

  // 设置外边框颜色为蓝色
  lv_style_set_border_color(&style_label_print_file,
                            lv_palette_main(LV_PALETTE_BLUE));
  // 设置填充
  lv_style_set_pad_all(&style_label_print_file, 2);

  lv_style_set_text_font(&style_label_print_file,
                         &font_28); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_print_file,
                          lv_color_hex(0xffffff)); // 设置样式文本字颜色

  lv_obj_add_style(label_print_file, &style_label_print_file,
                   LV_PART_MAIN); // 将样式添加到文字对象中

  lv_label_set_long_mode(label_print_file, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_width(label_print_file, 200);
  lv_obj_set_height(label_print_file, 200);

  lv_label_set_text(label_print_file, text_print_file_name.c_str());
  lv_obj_align(label_print_file, LV_ALIGN_CENTER, 0, 0);
}

//----------------------------------------screen4----初始化------------------------------------------------------//
void init_label_ap_config() {
  String TEXT = "AP Config....";

  label_ap_config = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_ap_config,
                         &font_20); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_ap_config,
                          lv_color_hex(0x2400FF)); // 设置样式文本字颜色

  lv_obj_add_style(label_ap_config, &style_label_ap_config,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_ap_config, TEXT.c_str());
  lv_obj_align(label_ap_config, LV_ALIGN_CENTER, 0, 0); // 居中显示
}

//----------------------------------------screen5----初始化------------------------------------------------------//
void init_label_no_klipper() {
  String TEXT = "No klipper connect";

  label_no_klipper = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_no_klipper,
                         &font_20); // 设置字体样机及大小
  lv_style_set_text_color(&style_label_no_klipper,
                          lv_color_hex(0x2400FF)); // 设置样式文本字颜色

  lv_obj_add_style(label_no_klipper, &style_label_no_klipper,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_no_klipper, TEXT.c_str());
  lv_obj_align(label_no_klipper, LV_ALIGN_CENTER, 0, 0); // 居中显示
}

//----------------------------------------screen6----初始化------------------------------------------------------//
void init_label_fan_speed() {
  String result;
  result = String(fanspeed_data);
  String TEXT = result;

  label_fan_speed = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_style_set_text_font(&style_label_fan_speed,
                         &font_24); // 设置字体样机及大小
  lv_style_set_text_color(
      &style_label_fan_speed,
      lv_palette_main(LV_PALETTE_RED)); // 设置样式文本字颜色

  lv_obj_add_style(label_fan_speed, &style_label_fan_speed,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_fan_speed, TEXT.c_str());
  lv_obj_align(label_fan_speed, LV_ALIGN_CENTER, 0, -40); // 居中显示
}

void init_bar_fan_speed() {
  bar_fan_speed = lv_bar_create(lv_scr_act()); // 创建圆弧对象

  lv_obj_set_style_arc_color(bar_fan_speed, lv_palette_main(LV_PALETTE_BLUE),
                             LV_PART_INDICATOR); // 进度条颜色

  lv_obj_set_size(bar_fan_speed, 200, 20);             // 设置尺寸
  lv_bar_set_range(bar_fan_speed, 0, 100);             // 设置开始结束
  lv_bar_set_value(bar_fan_speed, 0, LV_ANIM_OFF);     // 设置初始值
  lv_obj_align(bar_fan_speed, LV_ALIGN_CENTER, 0, 20); // 居中显示
}

//----------------------------------------screen1---刷新-------------------------------------------------------//
void update_label_print_progress() {

  String result;
  result = String(progress_data);
  String TEXT = result;

  if (progress_data == 0) {
    TEXT = "0%";
  } else {
    TEXT = TEXT + "%";
  }

  label_print_progress = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_print_progress, &style_label_print_progress,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_print_progress, TEXT.c_str());
  lv_obj_align(label_print_progress, LV_ALIGN_CENTER, 0, 0); // 居中显示
}

void update_arc_print_progress() {

  arc_print_progress = lv_arc_create(lv_scr_act()); // 创建圆弧对象

  lv_style_set_arc_width(&style_arc_print_progress, 24); // 设置样式的圆弧粗细
  lv_obj_add_style(arc_print_progress, &style_arc_print_progress,
                   LV_PART_MAIN); // 将样式应用到圆弧背景
  lv_obj_add_style(arc_print_progress, &style_arc_print_progress,
                   LV_PART_INDICATOR); // 将样式应用到圆弧前景

  lv_obj_remove_style(arc_print_progress, NULL, LV_PART_KNOB); // 移除样式
  lv_obj_clear_flag(arc_print_progress, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_arc_color(arc_print_progress, lv_color_hex(0xFF0000),
                             LV_PART_INDICATOR);       // 进度条颜色
  lv_obj_set_size(arc_print_progress, 240, 240);       // 设置尺寸
  lv_arc_set_rotation(arc_print_progress, 270);        // 设置零度位置
  lv_arc_set_bg_angles(arc_print_progress, 0, 360);    // 设置角度
  lv_arc_set_value(arc_print_progress, progress_data); // 设置值
  lv_obj_center(arc_print_progress);                   // 居中显示
}

//-----------------------------------------------screen2--刷新-----------------------------------------------------//
void update_label_extruder_actual_temp() {
  label_ext_actual_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_ext_actual_temp, &style_label_ext_actual_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_ext_actual_temp, text_ext_actual_temp.c_str());
  lv_obj_align(label_ext_actual_temp, LV_ALIGN_CENTER, 0, 75); // 居中显示
}

void update_label_extruder_target_temp() {
  label_ext_target_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_ext_target_temp, &style_label_ext_target_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_ext_target_temp, text_ext_target_temp.c_str());
  lv_obj_align(label_ext_target_temp, LV_ALIGN_CENTER, 0, -75); // 居中显示
}

void update_label_heaterbed_actual_temp() {
  label_bed_actual_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_bed_actual_temp, &style_label_bed_actual_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_bed_actual_temp, text_bed_actual_temp.c_str());
  lv_obj_align(label_bed_actual_temp, LV_ALIGN_CENTER, 0, 75); // 居中显示
}

void update_label_heaterbed_target_temp() {
  label_bed_target_temp = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_bed_target_temp, &style_label_bed_target_temp,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_bed_target_temp, text_bed_target_temp.c_str());
  lv_obj_align(label_bed_target_temp, LV_ALIGN_CENTER, 0, -75); // 居中显示
}

//----------------------------------------screen3---刷新-------------------------------------------------------//
void update_label_print_file() {
  label_print_file = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_print_file, &style_label_print_file,
                   LV_PART_MAIN); // 将样式添加到文字对象中

  lv_label_set_long_mode(label_print_file, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_width(label_print_file, 200);
  lv_label_set_text(label_print_file, text_print_file_name.c_str());
  lv_obj_align(label_print_file, LV_ALIGN_CENTER, 0, 0);
}

//----------------------------------------screen4---刷新-------------------------------------------------------//
void update_label_ap_config() {
  String TEXT = "AP Config....";

  label_ap_config = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_ap_config, &style_label_ap_config,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_ap_config, TEXT.c_str());
  lv_obj_align(label_ap_config, LV_ALIGN_CENTER, 0, 0); // 居中显示
}

//----------------------------------------screen5---刷新-------------------------------------------------------//
void update_label_no_klipper() {
  String TEXT = "No klipper connect";

  label_no_klipper = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_no_klipper, &style_label_no_klipper,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_no_klipper, TEXT.c_str());
  lv_obj_align(label_no_klipper, LV_ALIGN_CENTER, 0, 0); // 居中显示
}

//----------------------------------------screen6---刷新-------------------------------------------------------//
void update_label_fan_speed() {
  String result;
  result = String(fanspeed_data);
  String TEXT = result;

  if (fanspeed_data == 0) {
    TEXT = "fan speed: 0%";
  } else {
    TEXT = "fan speed: " + TEXT + "%";
  }

  label_fan_speed = lv_label_create(lv_scr_act()); // 创建文字对象

  lv_obj_add_style(label_fan_speed, &style_label_fan_speed,
                   LV_PART_MAIN); // 将样式添加到文字对象中
  lv_label_set_text(label_fan_speed, TEXT.c_str());
  lv_obj_align(label_fan_speed, LV_ALIGN_CENTER, 0, -40); // 居中显示
}

void update_bar_fan_speed() {
  bar_fan_speed = lv_bar_create(lv_scr_act()); // 创建圆弧对象

  lv_obj_set_style_arc_color(bar_fan_speed, lv_palette_main(LV_PALETTE_BLUE),
                             LV_PART_INDICATOR); // 进度条颜色

  lv_obj_set_size(bar_fan_speed, 200, 20); // 设置尺寸
  lv_bar_set_range(bar_fan_speed, 0, 100); // 设置开始结束
  lv_bar_set_value(bar_fan_speed, fanspeed_data, LV_ANIM_OFF); // 设置初始值
  lv_obj_align(bar_fan_speed, LV_ALIGN_CENTER, 0, 20);         // 居中显示
}

//-----------------------------------------------------------------------------------------------------//
void update_screen1(lv_timer_t *timer) {
  update_label_print_progress();
  update_arc_print_progress();

  exist_object_screen_flg = 1;
}

void update_screen7(lv_timer_t *timer) {
  ri_standby = KnownResourceImages::get_Standby();

  exist_object_screen_flg = 7;
}

void update_screen9(lv_timer_t *timer) {
  ri_print = KnownResourceImages::get_Printing();

  exist_object_screen_flg = 9;
}

void update_screen11(lv_timer_t *timer) {
  ri_bedTemp = KnownResourceImages::get_bed_temp();
  update_label_heaterbed_actual_temp();
  update_label_heaterbed_target_temp();

  exist_object_screen_flg = 11;
}

void update_screen12(lv_timer_t *timer) {
  ri_extTemp = KnownResourceImages::get_ext_temp();
  update_label_extruder_actual_temp();
  update_label_extruder_target_temp();

  exist_object_screen_flg = 12;
}

void update_screen14(lv_timer_t *timer) {
  ri_ok = KnownResourceImages::get_Print_ok();

  exist_object_screen_flg = 14;
}

void update_screen15(lv_timer_t *timer) {
  ri_voron = KnownResourceImages::get_Voron();

  exist_object_screen_flg = 15;
}

void update_screen18(lv_timer_t *timer) {
  ri_before = KnownResourceImages::get_BeforePrinting();

  exist_object_screen_flg = 18;
}

void update_screen19(lv_timer_t *timer) {
  ri_after = KnownResourceImages::get_AfterPrinting();

  exist_object_screen_flg = 19;
}

void update_screen21(lv_timer_t *timer) {
  ri_home = KnownResourceImages::get_Home();

  exist_object_screen_flg = 21;
}

void update_screen22(lv_timer_t *timer) {
  ri_level = KnownResourceImages::get_levelling();

  exist_object_screen_flg = 22;
}

void update_screen23(lv_timer_t *timer) {
  ri_disconnect = KnownResourceImages::get_Disconnect_Back();

  exist_object_screen_flg = 23;
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
  KeyScan();
}

void delete_exist_object() {
  if (exist_object_screen_flg == 1) { // del screen1

    lv_obj_del(label_print_progress);
    lv_obj_del(arc_print_progress);

  } else if (exist_object_screen_flg == 2) {

  } else if (exist_object_screen_flg == 3) {

    lv_obj_del(label_print_file);

  } else if (exist_object_screen_flg == 4) {

    lv_obj_del(label_ap_config);

  } else if (exist_object_screen_flg == 5) {

    lv_obj_del(label_no_klipper);

  } else if (exist_object_screen_flg == 6) {

    lv_obj_del(label_fan_speed);
    lv_obj_del(bar_fan_speed);

  } else if (exist_object_screen_flg == 7) {

    if (ri_standby) {
      delete ri_standby;
      ri_standby = nullptr;
    }
  } else if (exist_object_screen_flg == 9) {

    if (ri_print) {
      delete ri_print;
      ri_print = nullptr;
    }
  } else if (exist_object_screen_flg == 10) {
    if (ri_after) {
      delete ri_after;
      ri_after = nullptr;
    }
  } else if (exist_object_screen_flg == 11) {
    if (ri_bedTemp) {
      delete ri_bedTemp;
      ri_bedTemp = nullptr;
    }
    lv_obj_del(label_bed_actual_temp);
    lv_obj_del(label_bed_target_temp);
  } else if (exist_object_screen_flg == 12) {

    if (ri_extTemp) {
      delete ri_extTemp;
      ri_extTemp = nullptr;
    }
    lv_obj_del(label_ext_actual_temp);
    lv_obj_del(label_ext_target_temp);
  } else if (exist_object_screen_flg == 13) {

  } else if (exist_object_screen_flg == 14) {

    if (ri_ok) {
      delete ri_ok;
      ri_ok = nullptr;
    }
  } else if (exist_object_screen_flg == 15) {

    if (ri_voron) {
      delete ri_voron;
      ri_voron = nullptr;
    }
  } else if (exist_object_screen_flg == 18) {

    if (ri_before) {
      delete ri_before;
      ri_before = nullptr;
    }

  } else if (exist_object_screen_flg == 19) {

    if (ri_after) {
      delete ri_after;
      ri_after = nullptr;
    }
  } else if (exist_object_screen_flg == 21) {

    if (ri_home) {
      delete ri_home;
      ri_home = nullptr;
    }
  } else if (exist_object_screen_flg == 22) {

    if (ri_level) {
      delete ri_level;
      ri_level = nullptr;
    }
  } else if (exist_object_screen_flg == 23) {
    if (ri_disconnect)
      delete ri_disconnect;
  } else {
  }
}

const lv_font_t font_default = lv_font_t();

void Display_Object_Init() {
  init_label_print_status();
  lv_obj_del(label_print_status);

  init_label_print_progress();
  lv_obj_del(label_print_progress);

  init_arc_print_progress();
  lv_obj_del(arc_print_progress);

  init_label_extruder_actual_temp();
  lv_obj_del(label_ext_actual_temp);

  init_label_extruder_target_temp();
  lv_obj_del(label_ext_target_temp);

  init_label_heaterbed_actual_temp();
  lv_obj_del(label_bed_actual_temp);

  init_label_heaterbed_target_temp();
  lv_obj_del(label_bed_target_temp);

  init_label_print_file();
  lv_obj_del(label_print_file);

  init_label_ap_config();
  lv_obj_del(label_ap_config);

  init_label_no_klipper();
  lv_obj_del(label_no_klipper);

  init_label_fan_speed();
  lv_obj_del(label_fan_speed);
  init_bar_fan_speed();
  lv_obj_del(bar_fan_speed);

  delete KnownResourceImages::get_Standby();

  exist_object_screen_flg = 0;
  screen_begin_dis_flg = 0;
}

#if LV_USE_LOG
void logToSerial(const char* logLine)
{
    Serial.print(logLine);
}
#endif

__attribute__((unused)) void setup() {
  Serial.begin(115200); // 波特率
  EEPROM.begin(1024);   // 分配flash空间存储配网信息
#if LV_USE_LOG
  lv_log_register_print_cb(&logToSerial);
#endif
  delay(100);
  readwificonfig(); // 将wifi账号读出，判断是否进入配网界面

  if (wificonf.apmodeflag[0] != '8') { // 直接进入配网
    wifi_ap_config_flg = 1;
  }

  LV_LOG_INFO("SSID:%s\r\n", wificonf.stassid);

  InitKeyInterface(); // 按键接口初始化
  lv_display_Init();  // 显示初始化
  lv_port_littlefs_init();

  Display_Object_Init(); // 所有显示对象初始化一遍

  Open_display_init();

  lv_display_led_Init(); // 晚一点开背光

  timer1.attach(
      0.001, timer1_cb); // 定时0.001s，即1ms，回调函数为timer1_cb，并启动定时器

  if (wifi_ap_config_flg == 1) {
    wifiConfig(); // 开始配网功能
  }
}

bool otaWasInit = false;

__attribute__((unused)) void loop() {
  // lv_tick_inc(1);/* le the GUI do its work */
  lv_task_handler();

  if (screen_begin_dis_flg == 1) {
    //-------------HTTP请求-----------------------//
    httprequest_nowtime = millis();
    if (httprequest_nowtime > httprequest_nexttime) {

      if ((WiFi.status() == WL_CONNECTED) && (KeyDownFlag != KEY_DWON) &&
          (start_http_request_flg ==
           1)) { // wifi已经连接成功，发送http请求获取数据
        if (!otaWasInit) {
          otaWasInit = true;
          initOtaServer();
        }

        HTTPClient http;

        wifi_ap_config_flg = 0; // 已连接上wifi

        if (First_connection_flg == 0) { // 连接上wifi 切换回正常显示
          timer_contne = 0;
          display_step = 2;
          First_connection_flg = 1;
        }

        if (httpswitch == 1) {
          http.begin("http://" + klipper_ip + "/api/printer"); // 获取温度
        } else if (httpswitch == 2) {
          http.begin("http://" + klipper_ip +
                     "/printer/objects/query?display_status"); // 获取打印
        } else if (httpswitch == 3) {
          http.begin(
              "http://" + klipper_ip +
              "/printer/objects/query?gcode_macro%20G28"); // 获取home状态
        } else if (httpswitch == 4) {
          http.begin(
              "http://" + klipper_ip +
              "/printer/objects/query?gcode_macro%20BED_MESH_CALIBRATE"); // 获取levelling状态
        } else {
        }

        int httpCode = http.GET(); // Make the request

        if (httpCode > 0) { // Check for the returning code

          screen_no_klipper_dis_flg = 0;

          String payload = http.getString();
          DynamicJsonDocument doc(payload.length() * 2);
          deserializeJson(doc, payload);

          if (httpswitch == 1) { // 喷头热床温度显示

            String nameStr1 = doc["temperature"]["bed"]["actual"].as<String>();
            String nameStr2 = doc["temperature"]["bed"]["target"].as<String>();
            String nameStr3 =
                doc["temperature"]["tool0"]["actual"].as<String>();
            String nameStr4 =
                doc["temperature"]["tool0"]["target"].as<String>();
            String nameStr5 = doc["state"]["flags"]["printing"].as<String>();
            String nameStr6 = doc["state"]["flags"]["paused"].as<String>();

            bedtemp_actual =
                (uint16_t)((doc["temperature"]["bed"]["actual"].as<double>()) *
                           100);
            bedtemp_target =
                (uint16_t)((doc["temperature"]["bed"]["target"].as<double>()) *
                           100);
            tooltemp_actual = (uint16_t)((doc["temperature"]["tool0"]["actual"]
                                              .as<double>()) *
                                         100);
            tooltemp_target = (uint16_t)((doc["temperature"]["tool0"]["target"]
                                              .as<double>()) *
                                         100);

            LV_LOG_INFO(nameStr1);
            LV_LOG_INFO(nameStr2);
            LV_LOG_INFO(nameStr3);
            LV_LOG_INFO(nameStr4);

            text_ext_actual_temp = nameStr3 + "°C";
            text_ext_target_temp = nameStr4 + "°C";
            text_bed_actual_temp = nameStr1 + "°C";
            text_bed_target_temp = nameStr2 + "°C";

            if (nameStr5 == "true") {
              text_print_status = "Printing";
              print_status = 1;
            } else {
              if (nameStr6 == "true") {
                text_print_status = "paused";
                print_status = 2;
              } else {
                text_print_status = "standby";
                print_status = 0;
              }
            }

            if (print_status == 0) {
              if ((bedtemp_target > last_bedtemp_target) &&
                  (bedtemp_target != 0)) // 启动加热
              {
                timer_contne = 0;
                display_step = 3;
              }

              if ((tooltemp_target > last_tooltemp_target) &&
                  (tooltemp_target != 0)) // 启动加热
              {
                timer_contne = 0;
                display_step = 4;
              }
            }
            last_bedtemp_target = bedtemp_target;
            last_tooltemp_target = tooltemp_target;

            httpswitch = 2;
          } else if (httpswitch == 2) { // 打印进度

            double nameStr7 =
                (doc["result"]["status"]["display_status"]["progress"]
                     .as<double>()) *
                1000;
            uint16_t datas = (uint16_t)(nameStr7);
            uint16_t datas1 = datas % 10;

            if (datas1 > 4) {
              datas = (datas + 10) / 10;
            } else {
              datas = datas / 10;
            }

            progress_data = datas;
            if (datas == 0) {
              nameStrpriting = "0";
            } else {
              String result;
              result = String(datas);
              nameStrpriting = result;
            }
            LV_LOG_INFO(nameStrpriting);

            httpswitch = 3;
          } else if (httpswitch == 3) { // home状态

            String nameStr8 =
                doc["result"]["status"]["gcode_macro G28"]["homing"]
                    .as<String>();
            LV_LOG_INFO(nameStr8);

            if (nameStr8 == "true") {
              homing_status = 1;
              display_step = 12; // 更快进入显示
              timer_contne = 0;
            } else {
              homing_status = 0;
            }

            httpswitch = 4;
          } else if (httpswitch == 4) { // levelling状态

            String nameStr9 = doc["result"]["status"]
                                 ["gcode_macro BED_MESH_CALIBRATE"]["probing"]
                                     .as<String>();
            LV_LOG_INFO(nameStr9);

            if (nameStr9 == "true") {
              levelling_status = 1;
              display_step = 13; // 更快进入显示
              timer_contne = 0;
            } else {
              levelling_status = 0;
            }

            httpswitch = 1;
          } else {
          }

        } else {

          if (screen_no_klipper_dis_flg < 10)
            screen_no_klipper_dis_flg++;

          LV_LOG_INFO("Error on HTTP request");

          if (screen_no_klipper_dis_flg > 3) {
            display_step = 8; // no klipper connect
            timer_contne = 0;
          }
        }
        http.end(); // Free the resources
      }

      httprequest_nexttime = httprequest_nowtime + 97UL;
    }

    keyscan_nowtime = millis();
    if (keyscan_nowtime > keyscan_nexttime) {

      if (timer_contne > 0)
        timer_contne--; // 显示计时

      if (wifi_ap_config_flg == 0) {

        if ((display_step == 2) && (timer_contne == 0)) { // Standby
          timer_contne = 5;

          if (homing_status == 1) {
            timer_contne = 5;
            display_step = 12;
          } else if (levelling_status == 1) {
            timer_contne = 5;
            display_step = 13;
          } else if (print_status == 1) {
            timer_contne = 5;
            display_step = 3;
            standby_voron_dis_flg = 0;
          } else {
            if (standby_voron_dis_flg == 0) {

              standby_voron_dis_flg = 1;

              delete_exist_object();
              update_timer = lv_timer_create(update_screen7, 0, NULL);
              lv_timer_set_repeat_count(update_timer, 1);
            } else {
              display_step = 11; // to voron
            }
          }
        }

        if ((display_step == 11) && (timer_contne == 0)) { // voron
          timer_contne = 5;

          if (homing_status == 1) {
            timer_contne = 5;
            display_step = 12;
          } else if (levelling_status == 1) {
            timer_contne = 5;
            display_step = 13;
          } else if (print_status == 1) {
            timer_contne = 5;
            display_step = 3;
            standby_voron_dis_flg = 0;
          } else {
            if (standby_voron_dis_flg == 1) {

              standby_voron_dis_flg = 0;

              delete_exist_object();
              update_timer = lv_timer_create(update_screen15, 0, NULL);
              lv_timer_set_repeat_count(update_timer, 1);
            } else {
              display_step = 2; // to Standby
            }
          }
        }

        if ((display_step == 12) && (timer_contne == 0)) { // homing
          timer_contne = 5;

          if (homing_status == 0) {
            display_step = 2;
            timer_contne = 1;
          } else {
            delete_exist_object();
            update_timer = lv_timer_create(update_screen21, 0, NULL);
            lv_timer_set_repeat_count(update_timer, 1);
          }
        }

        if ((display_step == 13) && (timer_contne == 0)) { // levelling
          timer_contne = 5;

          if (levelling_status == 0) {
            display_step = 2;
          } else {
            delete_exist_object();
            update_timer = lv_timer_create(update_screen22, 0, NULL);
            lv_timer_set_repeat_count(update_timer, 1);
          }
        }

        if ((display_step == 3) && (timer_contne == 0)) {
          timer_contne = 5;

          if ((bedtemp_actual >= bedtemp_target) && (bedtemp_target != 0)) {
            display_step = 4;
          } else {

            if (bedtemp_target == 0) {
              display_step = 4;
            } else {
              delete_exist_object();
              update_timer = lv_timer_create(update_screen11, 0, NULL); // BED
              lv_timer_set_repeat_count(update_timer, 1);
            }
          }
        }

        if ((display_step == 4) && (timer_contne == 0)) {
          timer_contne = 5;

          if ((tooltemp_actual >= tooltemp_target) && (tooltemp_target != 0)) {

            if (print_status == 0) {
              display_step = 2;
            } else {
              display_step = 9;
              delete_exist_object();
              update_timer =
                  lv_timer_create(update_screen18, 0, NULL); // BeforePrinting
              lv_timer_set_repeat_count(update_timer, 1);
            }

          } else {

            if (tooltemp_target == 0) {
              display_step = 9;
            } else {
              delete_exist_object();
              update_timer = lv_timer_create(update_screen12, 0, NULL); // EXT
              lv_timer_set_repeat_count(update_timer, 1);
            }
          }
        }

        if ((display_step == 9) && (timer_contne == 0)) {
          timer_contne = 1;
          display_step = 5;
        }

        if ((display_step == 5) && (timer_contne == 0)) {
          timer_contne = 5;

          if (print_status == 1) {
            if (progress_data == 100) {
              display_step = 6;
              timer_contne = 7;
              delete_exist_object();
              update_timer =
                  lv_timer_create(update_screen14, 0, NULL); // print_ok
              lv_timer_set_repeat_count(update_timer, 1);
            } else {

              if (progress_data >= 1) {
                delete_exist_object();
                update_timer =
                    lv_timer_create(update_screen1, 0, NULL); // 过1%显示进度
                lv_timer_set_repeat_count(update_timer, 1);
              } else {
                delete_exist_object();
                update_timer =
                    lv_timer_create(update_screen9, 0, NULL); // printing
                lv_timer_set_repeat_count(update_timer, 1);
              }
            }
          } else {
            display_step = 2;
          }
        }

        if ((display_step == 6) && (timer_contne == 0)) {
          timer_contne = 5;
          display_step = 10;

          delete_exist_object();
          update_timer =
              lv_timer_create(update_screen19, 0, NULL); // AfterPrinting
          lv_timer_set_repeat_count(update_timer, 1);
        }

        if ((display_step == 10) && (timer_contne == 0)) {
          timer_contne = 5;
          display_step = 2;
        }

        if ((display_step == 8) && (timer_contne == 0)) { // no klipper connect
          timer_contne = 2;

          if (screen_no_klipper_dis_flg == 0) {
            display_step = 2;
          } else {
            delete_exist_object();
            update_timer = lv_timer_create(update_screen23, 0, NULL);
            lv_timer_set_repeat_count(update_timer, 1);
          }
        }
      }

      start_http_request_flg = 1; // 连接上wifi后 ， 启动http请求
      keyscan_nexttime = keyscan_nowtime + 400;
    }
  }

  //----------------网络连接检查，AP热点配网------------------//
  netcheck_nowtime = millis();
  if (netcheck_nowtime > netcheck_nexttime) {

    checkConnect(true); // 检测网络连接状态，参数true表示如果断开重新连接
    checkDNS_HTTP(); // 检测客户端DNS&HTTP请求，也就是检查配网页面那部分

    if (WiFi.status() != WL_CONNECTED) { // wifi没有连接成功
      First_connection_flg = 0;
    }
    netcheck_nexttime = netcheck_nowtime + 100UL;
  }
}
