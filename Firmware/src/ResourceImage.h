#pragma once
#include <Arduino.h>
#include <lvgl.h>
#include <string>

class ResourceImage {
private:
  lv_obj_t *img;

public:
  ResourceImage(const lv_img_dsc_t *image, String filename, lv_coord_t x, lv_coord_t y) {
    lv_obj_t *screen = lv_scr_act();
    if (filename.endsWith(".gif")) {
      Serial.println("Creating resource image " + filename + "as gif");
      img = lv_gif_create(screen);
      lv_gif_set_src(img, image);
    } else {
      Serial.println("Creating resource image " + filename + "as img");
      img = lv_img_create(screen);
      lv_img_set_src(img, image);
    }
    lv_obj_align(img, LV_ALIGN_CENTER, x, y);
  }

  ~ResourceImage() { lv_obj_del(img); }
};