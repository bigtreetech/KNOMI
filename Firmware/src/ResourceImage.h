#pragma once
#include <string>
#include <lvgl.h>

class ResourceImage {
  private: 
    lv_obj_t* img;

  public:
    ResourceImage(const lv_img_dsc_t* image) {
        img = lv_img_create(lv_scr_act());
        lv_img_set_src(img, image);
        lv_obj_align(img,LV_ALIGN_CENTER,0,0);
    }

    ~ResourceImage() {
        lv_obj_del(img);
    }
};