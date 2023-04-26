#include <lvgl_gui.h>
#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <WiFiUser.h>
#include <lvgl_logo_btt.h>
#include <lvgl_gif.h>
#include <test.h>

//显示刷新定时器
lv_timer_t * update_timer=NULL;
uint8_t contne = 0;
uint8_t print_status = 0;  //0 待机  1 打印  2暂停
uint8_t homing_status = 0;  
uint8_t levelling_status = 0;  
uint8_t timer_contne = 0;  //显示时间
uint8_t standby_voron_dis_flg = 0;  // 0 standby 1voron 循环显示
uint8_t display_step = 0;  //显示步骤流程 0 开机动画   2 待机动画 3 热床温度 4 喷头温度 5 打印中6 打印完成
                           //9 打印前 10 打印完成后  11 voron logo  12 homing  13 levelling
                           //7 apconfig  8 no klipper

uint8_t exist_object_screen_flg = 0; //0 没有存在的屏幕对象
uint8_t screen_begin_dis_flg = 0; //0 启动while循环
uint8_t screen_no_klipper_dis_flg = 0; //0 连接klipper失败
uint8_t start_http_request_flg = 0; //0 开始启动http请求
uint8_t wifi_ap_config_flg = 0; //0 wifi配网中
uint8_t First_connection_flg = 0; //第一次连接上

//打印界面对象定义
lv_obj_t * label_print_status;
lv_obj_t * label_print_progress;
lv_obj_t * arc_print_progress;

//打印界面对象样式定义
lv_style_t style_label_print_status;
lv_style_t style_label_print_progress;
lv_style_t style_arc_print_progress;

//温度界面对象定义
lv_obj_t * label_ext_actual_temp;
lv_obj_t * label_ext_target_temp;
lv_obj_t * label_bed_actual_temp; 
lv_obj_t * label_bed_target_temp; 

lv_obj_t * arc_extruder_temp; 
lv_obj_t * arc_heaterbed_temp; 

//温度界面对象样式定义
lv_style_t style_label_ext_actual_temp;
lv_style_t style_label_ext_target_temp;
lv_style_t style_label_bed_actual_temp;
lv_style_t style_label_bed_target_temp;

lv_style_t style_arc_extruder_temp; 
lv_style_t style_arc_heaterbed_temp; 

//打印文件界面对象定义
lv_obj_t * label_print_file;
//打印文件界面对象样式定义
lv_style_t style_label_print_file;

//配置界面对象定义
lv_obj_t * label_ap_config;
//配置界面对象样式定义
lv_style_t style_label_ap_config;

//klipper连接失败界面对象定义
lv_obj_t * label_no_klipper;
//klipper连接失败界面对象样式定义
lv_style_t style_label_no_klipper;

//打印界面对象定义
lv_obj_t * label_fan_speed;
lv_obj_t * bar_fan_speed;
//打印界面对象样式定义
lv_style_t style_label_fan_speed;
lv_style_t style_bar_fan_speed;

lv_obj_t * label_open_anim;
lv_obj_t * open_anim_arc;
lv_obj_t * open_back;//开机背景创建
lv_obj_t * label_open;//开机wifi连接提示标签
lv_style_t style_label_open;
lv_style_t style_label_load;
lv_style_t style_spinner_open;
lv_style_t style_bc_spinner_open;

lv_timer_t * timer_open=NULL;//用于延时处理开机问题：网络连接和动画
lv_timer_t * timer_open_task1=NULL;
lv_timer_t * timer_open_task2=NULL;
lv_timer_t * timer_open_init=NULL;
lv_timer_t * timer_project_init=NULL;

int wifi_connect_ok = 0; //wifi 连接成功标志
int wifi_connect_fail = 0; //wifi 连接失败标志

static void set_angle(void * var, int32_t v)
{
    lv_arc_set_value((lv_obj_t *)var, v);
}

void Open_up_animation()
{
    lv_style_set_arc_color(&style_spinner_open, lv_color_hex(0xFF0000)); //设置圆弧颜色
    lv_style_set_arc_width(&style_spinner_open, 16);            //设置圆弧宽度；
 
    lv_style_set_arc_color(&style_bc_spinner_open, lv_color_hex(0x000000)); //设置背景圆环颜色
    lv_style_set_arc_width(&style_bc_spinner_open, 16);        //设置背景圆环宽度
 
    open_anim_arc = lv_spinner_create(lv_scr_act(), 1000, 45);
 
    lv_obj_add_style(open_anim_arc, &style_spinner_open, LV_PART_INDICATOR);//应用到圆弧部分；
    lv_obj_add_style(open_anim_arc, &style_bc_spinner_open, LV_PART_MAIN);//应用到背景圆环部分；
 
    lv_obj_set_size(open_anim_arc, 240, 240);
    lv_obj_align(open_anim_arc, LV_ALIGN_CENTER, 0, 0);
}

void project_task_init(lv_timer_t * timer){

    lv_obj_del(open_anim_arc);
    lv_obj_del(img_open_logo);

    display_step = 2; //待机
    timer_contne = 1;

    delete_exist_object();
    update_timer = lv_timer_create(update_screen7, 0, NULL);
    lv_timer_set_repeat_count(update_timer,1);

    screen_begin_dis_flg = 1;
}

//成功连接 初始化项目 删除动画
void open_task_suc(lv_timer_t * timer){

    timer_project_init=lv_timer_create(project_task_init, 100, NULL);
    lv_timer_set_repeat_count(timer_project_init,1);
}

//初始化失败  重启
void open_task_err(lv_timer_t * timer){
    //  ESP.restart();

    timer_project_init=lv_timer_create(project_task_init, 100, NULL);
    lv_timer_set_repeat_count(timer_project_init,1);
}

//提示是否成功连接
void open_task_1(lv_timer_t * timer){

//成功连接wifi
    if(wifi_connect_ok == 1){
        timer_open_task1=lv_timer_create(open_task_suc, 4000, NULL);
        lv_timer_set_repeat_count(timer_open_task1,1);
    }
//未能成功连接wifi
    if(wifi_connect_fail == 1){
        timer_open_task2=lv_timer_create(open_task_err, 4000, NULL);
        lv_timer_set_repeat_count(timer_open_task2,1);
    }
}

void open_task_conv(lv_timer_t * timer){

    //连接wifi
    connectToWiFi(connectTimeOut_s);     //连接wifi，传入的是wifi连接等待时间15s    

    if(test_mode_flag == 0){

        init_img_open_logo_display();

        Open_up_animation();

        timer_open=lv_timer_create(open_task_1, 100, NULL);
        lv_timer_set_repeat_count(timer_open,1); 

    }
}

void Open_display_init(){

    if(test_mode_flag == 1){

    }else if(wifi_ap_config_flg == 1){
        update_gif_AP_Config_back_display();
        update_gif_AP_Config_display();
        exist_object_screen_flg = 20;
        screen_begin_dis_flg = 1;
    }else{

        init_img_open_logo_display();

        timer_open_init=lv_timer_create(open_task_conv, 100, NULL);
        lv_timer_set_repeat_count(timer_open_init,1);
    }

} 

