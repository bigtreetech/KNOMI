#ifndef LVGL_GIF_H
#define LVGL_GIF_H

#ifdef __cplusplus
extern "C" {  //extern "C"表示编译生成的内部符号名使用C约定。这样在c++文件中也可以调用对应c函数
#endif

#include <lvgl.h>

extern lv_obj_t * gif_White_back; 
extern lv_obj_t * img_black_back;

extern lv_obj_t * gif_AP_Config_back; 
extern lv_obj_t * gif_wait_back;

extern lv_obj_t * gif_Standby; 
extern lv_obj_t * gif_BeforePrinting; 
extern lv_obj_t * gif_AfterPrinting; 
extern lv_obj_t * gif_StartPrinting; 
extern lv_obj_t * gif_Printing; 
extern lv_obj_t * gif_PrintComplete; 

extern lv_obj_t * gif_bed_temp; 
extern lv_obj_t * gif_ext_temp; 

extern lv_obj_t * gif_OK; 
extern lv_obj_t * gif_voron; 

extern lv_obj_t * gif_AP_Config; 
extern lv_obj_t * gif_Home; 
extern lv_obj_t * gif_levelling; 
extern lv_obj_t * gif_wait;

void init_gif_AP_Config_display();
void init_gif_Home_display();
void init_gif_levelling_display();
void init_gif_wait_display();

void init_gif_White_back_display();
void init_gif_black_back_display();
void init_gif_Standby_display();
void init_gif_BeforePrinting_display();
void init_gif_AfterPrinting_display();
void init_gif_StartPrinting_display();
void init_gif_Printing_display();
void init_gif_PrintComplete_display();

void init_gif_bed_temp_display();
void init_gif_ext_temp_display();

void init_gif_OK_display();
void init_gif_voron_display();

void update_gif_AP_Config_display();
void update_gif_Home_display();
void update_gif_levelling_display();
void update_gif_wait_display();

void update_gif_White_back_display();
void update_gif_black_back_display();
void update_gif_Standby_display();
void update_gif_BeforePrinting_display();
void update_gif_AfterPrinting_display();
void update_gif_StartPrinting_display();
void update_gif_Printing_display();
void update_gif_PrintComplete_display();

void update_gif_AP_Config_back_display();
void update_gif_wait_back_display();

void update_gif_bed_temp_display();
void update_gif_ext_temp_display();

void update_gif_OK_display();
void update_gif_voron_display();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  
