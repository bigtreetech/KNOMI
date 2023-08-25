#pragma once
#include "lvgl.h"
#include "TFT_eSPI.h"
#include "Ticker.h"

class DisplayHAL {
  static lv_disp_draw_buf_t draw_buf;    // 定义显示器变量
  static lv_color_t buf[TFT_WIDTH * 10]; // 定义刷新缓存

  TFT_eSPI* tft;

public:
  DisplayHAL(Ticker* timer) {
    DisplayHAL::tft = new TFT_eSPI(240, 240);
    tft->init();
    tft->setRotation(0);
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, TFT_WIDTH * 10);

    static lv_disp_drv_t disp_drv;
    disp_drv.user_data = this;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = TFT_WIDTH;
    disp_drv.ver_res = TFT_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    pinMode(16, OUTPUT);
    digitalWrite(16, HIGH);

    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);

    timer->attach(0.001, DisplayHAL::Tick);
  }

  static void Tick() {
    lv_tick_inc(1); /* le the GUI do its work */
  }

  //------------------------------------------------------------------------------------------------------------//
  /* Display flushing */
  static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    auto that = (DisplayHAL*)disp->user_data;
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    that->tft->startWrite();                            // 使能写功能
    that->tft->setAddrWindow(area->x1, area->y1, w, h); // 设置填充区域
    that->tft->pushColors((uint16_t *)&color_p->full, w * h,
                   true); // 写入颜色缓存和缓存大小
    that->tft->endWrite();       // 关闭写功能

    lv_disp_flush_ready(disp); // 调用区域填充颜色函数
  }
};