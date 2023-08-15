#pragma once
#include <lvgl.h>
#include <string>

class ResourceImage {
private:
  lv_obj_t *img;

public:
  ResourceImage(const lv_img_dsc_t *image, bool isGif, int x, int y) {
    lv_obj_t *screen = lv_scr_act();
    if (isGif) {
      img = lv_gif_create(screen);
      lv_gif_set_src(img, &img);
    } else {
      img = lv_img_create(screen);
      lv_img_set_src(img, image);
    }
    lv_obj_align(img, LV_ALIGN_CENTER, x, y);
  }

  ~ResourceImage() { lv_obj_del(img); }
};

/*
 * white rectacngle example:
 *
 *  gif_White_back=lv_obj_create(lv_scr_act());//背景
    lv_obj_center(gif_White_back);
    lv_obj_set_size(gif_White_back,240,240);
    lv_obj_set_style_bg_color(gif_White_back,lv_color_hex(0xFFFFFF),NULL);
    lv_obj_align(gif_White_back,LV_ALIGN_CENTER,0,0);
 *
 * */