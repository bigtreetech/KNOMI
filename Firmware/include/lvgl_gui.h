#ifndef LVGL_GUI_H
#define LVGL_GUI_H

#ifdef __cplusplus
extern "C" {  //extern "C"表示编译生成的内部符号名使用C约定。这样在c++文件中也可以调用对应c函数
#endif

#include <lvgl.h>

extern int wifi_connect_ok; //wifi 连接成功标志
extern int wifi_connect_fail;  //wifi 连接失败标志

void Open_up_animation();    
void Open_display_init();


//显示刷新定时器
extern lv_timer_t * update_timer;
extern uint8_t contne;
extern uint8_t print_status; 
extern uint8_t homing_status;  
extern uint8_t levelling_status; 
extern uint8_t timer_contne;
extern uint8_t display_step;
extern uint8_t standby_voron_dis_flg; 

extern uint8_t exist_object_screen_flg; //0 没有存在的屏幕对象
extern uint8_t screen_begin_dis_flg;
extern uint8_t screen_no_klipper_dis_flg; //0 连接klipper失败
extern uint8_t start_http_request_flg; //0 开始启动http请求
extern uint8_t wifi_ap_config_flg; //0 wifi配网中
extern uint8_t First_connection_flg;

//打印界面对象定义
extern lv_obj_t * label_print_status;
extern lv_obj_t * label_print_progress;
extern lv_obj_t * arc_print_progress;
//打印界面对象样式定义
extern lv_style_t style_label_print_status;
extern lv_style_t style_label_print_progress;
extern lv_style_t style_arc_print_progress;

//温度界面对象定义
extern lv_obj_t * label_ext_actual_temp;
extern lv_obj_t * label_ext_target_temp;
extern lv_obj_t * label_bed_actual_temp; 
extern lv_obj_t * label_bed_target_temp; 

extern lv_obj_t * arc_extruder_temp; 
extern lv_obj_t * arc_heaterbed_temp; 
//温度界面对象样式定义
extern lv_style_t style_label_ext_actual_temp;
extern lv_style_t style_label_ext_target_temp;
extern lv_style_t style_label_bed_actual_temp;
extern lv_style_t style_label_bed_target_temp;

extern lv_style_t style_arc_extruder_temp; 
extern lv_style_t style_arc_heaterbed_temp; 

//打印文件界面对象定义
extern lv_obj_t * label_print_file;
//打印文件界面对象样式定义
extern lv_style_t style_label_print_file;

//配置界面对象定义
extern lv_obj_t * label_ap_config;
//配置界面对象样式定义
extern lv_style_t style_label_ap_config;

//klipper连接失败界面对象定义
extern lv_obj_t * label_no_klipper;
//klipper连接失败界面对象样式定义
extern lv_style_t style_label_no_klipper;

//打印界面对象定义
extern lv_obj_t * label_fan_speed;
extern lv_obj_t * bar_fan_speed;
//打印界面对象样式定义
extern lv_style_t style_label_fan_speed;
extern lv_style_t style_bar_fan_speed;

//界面对象及样式初始化函数定义
//---------screen1---------------//
void init_label_print_status();
void init_label_print_progress();
void init_arc_print_progress();
//---------screen2---------------//
void init_label_extruder_actual_temp();
void init_label_heaterbed_actual_temp();
void init_label_extruder_target_temp();
void init_label_heaterbed_target_temp();
void init_arc_extruder_temp();
void init_arc_heaterbed_temp();
//---------screen3---------------//
void init_label_print_file();
//---------screen4---------------//
void init_label_ap_config();
//---------screen5---------------//
void init_label_no_klipper();
//---------screen6---------------//
void init_label_fan_speed();

//界面对象刷新处理函数定义
//---------screen1---------------//
void update_label_print_status();
void update_label_print_progress();
void update_arc_print_progress();
//---------screen2---------------//
void update_label_extruder_actual_temp();
void update_label_heaterbed_actual_temp();
void update_label_extruder_target_temp();
void update_label_heaterbed_target_temp();
void update_arc_extruder_temp();
void update_arc_heaterbed_temp();
//---------screen3---------------//
void update_label_print_file();
//---------screen4---------------//
void update_label_ap_config();
//---------screen5---------------//
void update_label_no_klipper();
//---------screen6---------------//
void update_label_fan_speed();

//界面对象刷新处理函数定义
void update_screen1(lv_timer_t * timer);
void update_screen2(lv_timer_t * timer);
void update_screen3(lv_timer_t * timer);
void update_screen4(lv_timer_t * timer);
void update_screen5(lv_timer_t * timer);
void update_screen6(lv_timer_t * timer);
void update_screen7(lv_timer_t * timer);
void update_screen8(lv_timer_t * timer);
void update_screen9(lv_timer_t * timer);
void update_screen10(lv_timer_t * timer);
void update_screen11(lv_timer_t * timer);
void update_screen12(lv_timer_t * timer);
void update_screen13(lv_timer_t * timer);

//删除界面对象
void delete_exist_object();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  
