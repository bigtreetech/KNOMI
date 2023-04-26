#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {  //extern "C"表示编译生成的内部符号名使用C约定。这样在c++文件中也可以调用对应c函数
#endif

#include <lvgl.h>

//进入测试模式标志位
extern uint8_t test_mode_flag;
extern uint8_t test_key_cnt;
extern uint32_t test_key_timer_cnt;

extern lv_obj_t * img_red_test;
extern lv_obj_t * img_green_test;
extern lv_obj_t * img_blue_test;

void update_red_back_display();
void update_green_back_display();
void update_blue_back_display();

void update_label_scan_networks_test();
void update_label_networksID_test();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  
