#pragma once
#include <Arduino.h>
#include <log.h>
#include <string>

class ResourceImage {
private:
  String filename;

public:
  ResourceImage(String filename, int x, int y) {
    this->filename = filename;
    /* todo: AnimatedGIF
    lv_obj_t *screen = lv_scr_act();
    filename = "A:/" + filename;
    LV_LOG_INFO(("Creating resource image " + filename).c_str());
    if (filename.endsWith(".gif")) {
      img = lv_gif_create(screen);
      lv_gif_set_src(img, filename.begin());
    } else {
      // TODO in case of BMP manually decode image and store it in ram - this
      // may increase speed a lot.
      img = lv_img_create(screen);
      lv_img_set_src(img, filename.begin());
    }
    lv_obj_align(img, LV_ALIGN_CENTER, x, y);
    if (!filename.endsWith(".gif")) {
      lv_task_handler();
    }
    LV_LOG_INFO(("Created resource image " + filename).c_str());*/
  }

  ~ResourceImage() {
    /* todo: clean gif
    LV_LOG_INFO(("Deleting resource image " + this->filename).c_str());
    lv_obj_del(img);
    LV_LOG_INFO(("Deleted resource image " + this->filename).c_str());
     */
  }
};