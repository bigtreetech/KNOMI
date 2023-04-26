#include <lvgl_logo_btt.h>
#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFiUser.h>

LV_IMG_DECLARE(BTT_LOGO);

//开机界面对象定义
lv_obj_t * img_open_logo;


void init_img_open_logo_display()
{
  img_open_logo = lv_img_create(lv_scr_act());
  lv_img_set_src(img_open_logo, &BTT_LOGO);lv_obj_align(img_open_logo,LV_ALIGN_CENTER,0,0);
}




