#include <lvgl_gif.h>
#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFiUser.h>

LV_IMG_DECLARE(AP_Config);
LV_IMG_DECLARE(Home);
LV_IMG_DECLARE(levelling);
LV_IMG_DECLARE(wait);

LV_IMG_DECLARE(AP_Config_Back);
LV_IMG_DECLARE(Disconnect_Back);

LV_IMG_DECLARE(Standby);
LV_IMG_DECLARE(StartPrinting);
LV_IMG_DECLARE(Printing);
LV_IMG_DECLARE(PrintComplete);

LV_IMG_DECLARE(BeforePrinting);
LV_IMG_DECLARE(AfterPrinting);

LV_IMG_DECLARE(bc_black);

LV_IMG_DECLARE(bed_temp);
LV_IMG_DECLARE(ext_temp);

LV_IMG_DECLARE(Print_ok);
LV_IMG_DECLARE(Voron);


lv_obj_t * gif_AP_Config; 
lv_obj_t * gif_Home; 
lv_obj_t * gif_levelling; 
lv_obj_t * gif_wait;

lv_obj_t * gif_White_back; 
lv_obj_t * img_black_back;


lv_obj_t * gif_AP_Config_back; 
lv_obj_t * gif_wait_back;

lv_obj_t * gif_BeforePrinting; 
lv_obj_t * gif_AfterPrinting; 

lv_obj_t * gif_Standby; 
lv_obj_t * gif_StartPrinting; 
lv_obj_t * gif_Printing; 
lv_obj_t * gif_PrintComplete;

lv_obj_t * gif_bed_temp; 
lv_obj_t * gif_ext_temp; 

lv_obj_t * gif_OK; 
lv_obj_t * gif_voron; 

//-----------------------------------------白色背景图片对象定义-------------------------------------------//
void init_gif_White_back_display()
{
  gif_White_back=lv_obj_create(lv_scr_act());//背景
  lv_obj_center(gif_White_back);
  lv_obj_set_size(gif_White_back,240,240);
  lv_obj_set_style_bg_color(gif_White_back,lv_color_hex(0xFFFFFF),NULL);
  lv_obj_align(gif_White_back,LV_ALIGN_CENTER,0,0);
}

void update_gif_White_back_display()
{
  gif_White_back=lv_obj_create(lv_scr_act());//背景
  lv_obj_center(gif_White_back);
  lv_obj_set_size(gif_White_back,240,240);
  lv_obj_set_style_bg_color(gif_White_back,lv_color_hex(0xFFFFFF),NULL);
  lv_obj_align(gif_White_back,LV_ALIGN_CENTER,0,0);
}

//-----------------------------------------黑色背景图片对象定义-------------------------------------------//
void init_gif_black_back_display()
{
  img_black_back = lv_img_create(lv_scr_act());
  lv_img_set_src(img_black_back, &bc_black);lv_obj_align(img_black_back,LV_ALIGN_CENTER,0,0);
}

void update_gif_black_back_display()
{
  img_black_back = lv_img_create(lv_scr_act());
  lv_img_set_src(img_black_back, &bc_black);lv_obj_align(img_black_back,LV_ALIGN_CENTER,0,0);
}

void update_gif_AP_Config_back_display()
{
  gif_AP_Config_back = lv_img_create(lv_scr_act());
  lv_img_set_src(gif_AP_Config_back, &AP_Config_Back);lv_obj_align(gif_AP_Config_back,LV_ALIGN_CENTER,0,0);
}

void update_gif_wait_back_display()
{
  gif_wait_back = lv_img_create(lv_scr_act());
  lv_img_set_src(gif_wait_back, &Disconnect_Back);lv_obj_align(gif_wait_back,LV_ALIGN_CENTER,0,0);
}

//------------------------------------------图片显示初始化-------------------------------------------------//
void init_gif_AP_Config_display()
{
  gif_AP_Config = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_AP_Config, &AP_Config);
  lv_obj_align(gif_AP_Config,LV_ALIGN_CENTER,0,30);
}

void init_gif_Home_display()
{
  gif_AP_Config = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_AP_Config, &Home);
  lv_obj_align(gif_AP_Config,LV_ALIGN_CENTER,0,0);
}

void init_gif_levelling_display()
{
  gif_AP_Config = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_AP_Config, &levelling);
  lv_obj_align(gif_AP_Config,LV_ALIGN_CENTER,0,0);
}

void init_gif_wait_display()
{
  gif_wait = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_wait, &wait);
  lv_obj_align(gif_wait,LV_ALIGN_CENTER,0,0);
}

void init_gif_Standby_display()
{
  gif_Standby = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_Standby, &Standby);
  lv_obj_align(gif_Standby,LV_ALIGN_CENTER,0,0);
}

void init_gif_BeforePrinting_display()
{
  gif_BeforePrinting = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_BeforePrinting, &BeforePrinting);
  lv_obj_align(gif_BeforePrinting,LV_ALIGN_CENTER,0,0);
}

void init_gif_AfterPrinting_display()
{
  gif_AfterPrinting = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_AfterPrinting, &AfterPrinting);
  lv_obj_align(gif_AfterPrinting,LV_ALIGN_CENTER,0,0);
}

void init_gif_StartPrinting_display()
{
  gif_StartPrinting = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_StartPrinting, &StartPrinting);
  lv_obj_align(gif_StartPrinting,LV_ALIGN_CENTER,0,0);
}

void init_gif_Printing_display()
{
  gif_Printing = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_Printing, &Printing);
  lv_obj_align(gif_Printing,LV_ALIGN_CENTER,0,0);
}

void init_gif_PrintComplete_display()
{
  gif_PrintComplete = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_PrintComplete, &PrintComplete);
  lv_obj_align(gif_PrintComplete,LV_ALIGN_CENTER,0,0);
}

void init_gif_bed_temp_display()
{
  gif_bed_temp = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_bed_temp, &bed_temp);
  lv_obj_align(gif_bed_temp,LV_ALIGN_CENTER,0,0);
}

void init_gif_ext_temp_display()
{
  gif_ext_temp = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_ext_temp, &ext_temp);
  lv_obj_align(gif_ext_temp,LV_ALIGN_CENTER,0,0);
}

//---------------------------------------------------------------//
void init_gif_OK_display()
{
  gif_OK = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_OK, &Print_ok);
  lv_obj_align(gif_OK,LV_ALIGN_CENTER,0,0);
}

void init_gif_voron_display()
{
  gif_voron = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_voron, &Voron);
  lv_obj_align(gif_voron,LV_ALIGN_CENTER,0,0);
}

//----------------------------------------图片显示---刷新-------------------------------------------------------//
void update_gif_Standby_display()
{
  gif_Standby = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_Standby, &Standby);
  lv_obj_align(gif_Standby,LV_ALIGN_CENTER,0,0);
}

void update_gif_BeforePrinting_display()
{
  gif_BeforePrinting = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_BeforePrinting, &BeforePrinting);
  lv_obj_align(gif_BeforePrinting,LV_ALIGN_CENTER,0,0);
}

void update_gif_AfterPrinting_display()
{
  gif_AfterPrinting = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_AfterPrinting, &AfterPrinting);
  lv_obj_align(gif_AfterPrinting,LV_ALIGN_CENTER,0,0);
}

void update_gif_StartPrinting_display()
{
  gif_StartPrinting = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_StartPrinting, &StartPrinting);
  lv_obj_align(gif_StartPrinting,LV_ALIGN_CENTER,0,0);
}

void update_gif_Printing_display()
{
  gif_Printing = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_Printing, &Printing);
  lv_obj_align(gif_Printing,LV_ALIGN_CENTER,0,0);
}

void update_gif_PrintComplete_display()
{
  gif_PrintComplete = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_PrintComplete, &PrintComplete);
  lv_obj_align(gif_PrintComplete,LV_ALIGN_CENTER,0,0);
}

void update_gif_bed_temp_display()
{
  gif_bed_temp = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_bed_temp, &bed_temp);
  lv_obj_align(gif_bed_temp,LV_ALIGN_CENTER,0,0);
}

void update_gif_ext_temp_display()
{
  gif_ext_temp = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_ext_temp, &ext_temp);
  lv_obj_align(gif_ext_temp,LV_ALIGN_CENTER,0,0);
}

//-----------------------------------------------------------//
void update_gif_AP_Config_display()
{
  gif_AP_Config = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_AP_Config, &AP_Config);
  lv_obj_align(gif_AP_Config,LV_ALIGN_CENTER,0,-36);
}

void update_gif_Home_display()
{
  gif_Home = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_Home, &Home);
  lv_obj_align(gif_Home,LV_ALIGN_CENTER,0,0);
}

void update_gif_levelling_display()
{
  gif_levelling = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_levelling, &levelling);
  lv_obj_align(gif_levelling,LV_ALIGN_CENTER,0,0);
}

void update_gif_wait_display()
{
  gif_wait = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_wait, &wait);
  lv_obj_align(gif_wait,LV_ALIGN_CENTER,0,0);
}

void update_gif_OK_display()
{
  gif_OK = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_OK, &Print_ok);
  lv_obj_align(gif_OK,LV_ALIGN_CENTER,0,0);
}

void update_gif_voron_display()
{
  gif_voron = lv_gif_create(lv_scr_act());
  lv_gif_set_src(gif_voron, &Voron);
  lv_obj_align(gif_voron,LV_ALIGN_CENTER,0,0);
}


