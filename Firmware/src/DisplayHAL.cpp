#include "DisplayHAL.h"

lv_disp_draw_buf_t DisplayHAL::draw_buf;    // 定义显示器变量
lv_color_t DisplayHAL::buf[TFT_WIDTH * 10]; // 定义刷新缓存
