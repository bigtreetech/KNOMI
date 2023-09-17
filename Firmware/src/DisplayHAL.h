#pragma once
#include "TFT_eSPI.h"
#include "Ticker.h"
#include "lvgl.h"

class DisplayHAL {
  lv_disp_draw_buf_t draw_buf; // 定义显示器变量
  lv_disp_drv_t disp_drv;
  lv_color_t *buf; // 定义刷新缓存
  TFT_eSPI *tft;

#define TFT_BUFFER_SIZE (TFT_WIDTH * 10)

public:
  DisplayHAL() {
    tft = new TFT_eSPI(240, 240);
    tft->init();
    tft->fillScreen(0);
    tft->setRotation(0);
    lv_init();
    buf = new lv_color_t[TFT_BUFFER_SIZE];
    memset(buf, 0, sizeof(lv_color_t) * TFT_BUFFER_SIZE);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_BUFFER_SIZE);
    lv_disp_drv_init(&disp_drv);
    disp_drv.user_data = tft;
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);

    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
  }

  //------------------------------------------------------------------------------------------------------------//
  /* Display flushing */
  static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    auto that = (TFT_eSPI *)disp->user_data;
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    that->startWrite();                            // 使能写功能
    that->setAddrWindow(area->x1, area->y1, w, h); // 设置填充区域
    that->pushColors((uint16_t *)&color_p->full, w * h,
                     true); // 写入颜色缓存和缓存大小
    that->endWrite();       // 关闭写功能

    lv_disp_flush_ready(disp); // 调用区域填充颜色函数
  }
};