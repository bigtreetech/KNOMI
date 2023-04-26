#include <lvgl_gui.h>
#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFiUser.h>
#include <lvgl_logo_btt.h>
#include <lvgl_gif.h>
#include <test.h>


LV_IMG_DECLARE(red_test);
LV_IMG_DECLARE(green_test);
LV_IMG_DECLARE(blue_test);

//进入测试模式标志位
uint8_t test_mode_flag = 0;
uint8_t test_key_cnt = 0;
uint32_t test_key_timer_cnt = 0;

//测试界面对象定义
lv_obj_t * label_test;
lv_obj_t * label_netID1_test;
lv_obj_t * label_netID2_test;
lv_obj_t * label_netID3_test;

lv_obj_t * black_test;
lv_obj_t * white_test;
lv_obj_t * img_red_test;
lv_obj_t * img_green_test;
lv_obj_t * img_blue_test;

//测试界面对象样式定义
lv_style_t style_label_test;

String scan_networks_test = "Scan network"; 
String networksID_test = ""; 

void update_label_scan_networks_test()
{
  label_test = lv_label_create(lv_scr_act()); //创建文字对象

  lv_obj_add_style(label_test,&style_label_ap_config,LV_PART_MAIN);           //将样式添加到文字对象中
  lv_label_set_text(label_test,scan_networks_test.c_str());
  lv_obj_align(label_test, LV_ALIGN_CENTER, 0, -50); //居中显示
}

void update_label_networksID_test()
{
  networksID_test = scanNetworksID1; 

  label_netID1_test = lv_label_create(lv_scr_act()); //创建文字对象

  lv_obj_add_style(label_netID1_test,&style_label_ap_config,LV_PART_MAIN);           //将样式添加到文字对象中
  lv_label_set_text(label_netID1_test,networksID_test.c_str());
  lv_obj_align(label_netID1_test, LV_ALIGN_CENTER, 0, 0); //居中显示

  label_netID2_test = lv_label_create(lv_scr_act()); //创建文字对象

  lv_obj_add_style(label_netID2_test,&style_label_ap_config,LV_PART_MAIN);           //将样式添加到文字对象中
  lv_label_set_text(label_netID2_test,scanNetworksID2.c_str());
  lv_obj_align(label_netID2_test, LV_ALIGN_CENTER, 0, 30); //居中显示

  label_netID3_test = lv_label_create(lv_scr_act()); //创建文字对象

  lv_obj_add_style(label_netID3_test,&style_label_ap_config,LV_PART_MAIN);           //将样式添加到文字对象中
  lv_label_set_text(label_netID3_test,scanNetworksID3.c_str());
  lv_obj_align(label_netID3_test, LV_ALIGN_CENTER, 0, 60); //居中显示

}

void update_red_back_display()
{
  img_red_test = lv_img_create(lv_scr_act());
  lv_img_set_src(img_red_test, &red_test);lv_obj_align(img_red_test,LV_ALIGN_CENTER,0,0);
}

void update_green_back_display()
{
  img_green_test = lv_img_create(lv_scr_act());
  lv_img_set_src(img_green_test, &green_test);lv_obj_align(img_green_test,LV_ALIGN_CENTER,0,0);
}

void update_blue_back_display()
{
  img_blue_test = lv_img_create(lv_scr_act());
  lv_img_set_src(img_blue_test, &blue_test);lv_obj_align(img_blue_test,LV_ALIGN_CENTER,0,0);
}


