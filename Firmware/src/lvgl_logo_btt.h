#ifndef LVGL_LOGO_BTT_H
#define LVGL_LOGO_BTT_H

#ifdef __cplusplus
extern "C" {  //extern "C"表示编译生成的内部符号名使用C约定。这样在c++文件中也可以调用对应c函数
#endif

#include <lvgl.h>

extern lv_obj_t * img_open_logo;


void init_img_open_logo_display();


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  
