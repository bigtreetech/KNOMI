#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <string>

class ResourceImage {
private:
  lv_obj_t *img;

public:
  ResourceImage(String filename, lv_coord_t x, lv_coord_t y) {
    lv_obj_t *screen = lv_scr_act();
    filename = "A:/" + filename;
    LV_LOG_INFO(("Creating resource image " + filename + " as gif").c_str());
    if (filename.endsWith(".gif")) {
      img = lv_gif_create(screen);
      lv_gif_set_src(img, filename.begin());
    } else {
      // TODO in case of BMP manually decode image and store it in ram - this may increase speed a lot.
      img = lv_img_create(screen);
      lv_img_set_src(img, filename.begin());
    }
    lv_obj_align(img, LV_ALIGN_CENTER, x, y);
  }

  ~ResourceImage() { lv_obj_del(img); }
};