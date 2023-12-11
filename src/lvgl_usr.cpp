#include <stdio.h> // sprintf
#include <Arduino.h>

#include "lvgl_hal.h"
#include "lvgl_usr.h"
#include "ui/ui.h"
#include "moonraker.h"
#include "ui_overlay/lv_overlay.h"


/****************** lvgl ui call function ******************/
//
void lv_tft_set_backlight(lv_event_t * e) {
    int32_t light = lv_slider_get_value(ui_slider_backlight);
    tft_set_backlight(light);
}

// extruder speed
void lv_btn_set_extrude(lv_event_t * e) {
    // Initialize parameter values from roller settings
    char roller_str[10];
    lv_roller_get_selected_str(ui_roller_set_extrude_length, roller_str, sizeof(roller_str));
    lv_label_set_text(ui_label_extruder_length, roller_str);
    uint32_t sel = lv_roller_get_selected(ui_roller_set_extrude_length);
    lv_obj_set_user_data(ui_label_extruder_length, (void *)sel);
    lv_roller_get_selected_str(ui_roller_set_extrude_speed, roller_str, sizeof(roller_str));
    lv_label_set_text(ui_label_extruder_speed, roller_str);
    sel = lv_roller_get_selected(ui_roller_set_extrude_speed);
    lv_obj_set_user_data(ui_label_extruder_speed, (void *)sel);
}

// set extruder roller
void lv_roller_set_extrude(lv_event_t * e) {
    // Initialize parameter values from roller settings
    uint32_t sel = (uint32_t)lv_obj_get_user_data(ui_label_extruder_length);
    lv_roller_set_selected(ui_roller_set_extrude_length, sel, LV_ANIM_OFF);

    sel = (uint32_t)lv_obj_get_user_data(ui_label_extruder_speed);
    lv_roller_set_selected(ui_roller_set_extrude_speed, sel, LV_ANIM_OFF);
}
/***********************************************************/


void lv_popup_warning(const char * warning, bool clickable);
void lv_popup_remove(lv_event_t * e) ;
// lvgl ui
void lvgl_ui_task(void * parameter) {
    lv_btn_init();
    lvgl_hal_init();
    ui_init();

#ifndef LIS2DW_SUPPORT
    // progress in center if no lis2dw accelerometer data to display
    lv_obj_set_y(ui_label_printing_progress, 0);
    lv_obj_set_align(ui_label_printing_progress, LV_ALIGN_CENTER);
    // delete unused accelerometer data
    lv_obj_del(ui_slider_printing_acc_x);
    lv_obj_del(ui_slider_printing_acc_y);
    lv_obj_del(ui_slider_printing_acc_z);
    lv_obj_del(ui_label_printing_acc_x);
    lv_obj_del(ui_label_printing_acc_y);
    lv_obj_del(ui_label_printing_acc_z);
#endif

    lv_obj_t * label = lv_label_create(ui_ScreenTestImg);
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -30);
    lv_label_set_text_static(label, FW_VERSION);

    // Add all button style
    lv_btn_add_style();

    // Set theme color
    lv_theme_color_style();

    // Add logo gif
    ui_img_main_gif = lv_gif_create(ui_ScreenMainGif);
    lv_gif_set_src(ui_img_main_gif, gif_idle[0]);
    lv_obj_align(ui_img_main_gif, LV_ALIGN_CENTER, 0, 0);

    // Add welcome gif
    lv_obj_t * img_welcome_gif = lv_gif_create(ui_ScreenWelcome);
    lv_gif_set_src(img_welcome_gif, &gif_welcome);
    lv_obj_align(img_welcome_gif, LV_ALIGN_CENTER, 0, -36);

    // Create a QR Code
    lv_obj_t * qr = lv_qrcode_create(ui_ScreenQRCode, 130, LV_COLOR_MAKE(0xff, 0xff, 0xff), LV_COLOR_MAKE(0, 0, 0));
    const char * data = "https://bigtreetech.github.io/docs/KNOMI2.html";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);

    // Initialize extruder speed/length roller options
    const char *extrude_len = {
        EXTRUDE_MM_0_LABEL "\n"\
        EXTRUDE_MM_1_LABEL "\n"\
        EXTRUDE_MM_2_LABEL "\n"\
        EXTRUDE_MM_3_LABEL "\n"\
        EXTRUDE_MM_4_LABEL
    };
    lv_roller_set_options(ui_roller_set_extrude_length, extrude_len, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(ui_roller_set_extrude_length, 1, LV_ANIM_ON); // 5mm
    const char *EXTRUDE_MM_S = {
        EXTRUDE_MM_S_0_LABEL "\n"\
        EXTRUDE_MM_S_1_LABEL "\n"\
        EXTRUDE_MM_S_2_LABEL "\n"\
        EXTRUDE_MM_S_3_LABEL "\n"\
        EXTRUDE_MM_S_4_LABEL
    };
    lv_roller_set_options(ui_roller_set_extrude_speed, EXTRUDE_MM_S, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(ui_roller_set_extrude_speed, 2, LV_ANIM_ON); // 10mm/s
    // Initialize extruder speed/length values from roller settings
    lv_btn_set_extrude(NULL);

    for(;;) {
        // lvgl task, must run in loop first.
        lv_timer_handler();

        wifi_status_t status = wifi_get_connect_status();

        lv_loop_wifi_change_screen(status);

        if (status == WIFI_STATUS_CONNECTED) {

            lv_loop_popup_screen();
            lv_loop_set_temp_screen();

            if (!moonraker.unconnected && !moonraker.unready) {
                if (moonraker.data_unlock) {
                    lv_loop_moonraker_change_screen();
                }
                lv_loop_moonraker_change_screen_value();
            }
        }

        lv_loop_auto_idle(status);
        lv_loop_btn_event();

        delay(5);
    }
}
