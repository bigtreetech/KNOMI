#include "ui/ui.h"
#include "knomi.h"
#include "lv_overlay.h"

void lv_theme_update_color(lv_color_t c) {
    knomi_config.theme_color = c;
    knomi_config_require_change(LOCAL_POST_LV_THEME_COLOR);
    lv_btn_add_style();
    lv_theme_color_style();
}

lv_color_t lv_theme_color(void) {
    return knomi_config.theme_color;
}

/**
 * Theme color for img
 */
void lv_theme_color_style(void) {
    // default not opa, display of the original color of the image
    lv_opa_t opa = (knomi_config.theme_color.full == lv_color_hex(LV_DEFAULT_COLOR).full) ? 0 : 255;

    lv_obj_t * const img_obj[] = {
        // ScreenExtrude
        ui_btn_extruder_retract, ui_btn_extruder_extrude, ui_btn_extruder_temp, ui_btn_extruder_speed,
        // ScreenMove
        ui_btn_move_abl, ui_btn_move_qgl, ui_btn_move_home,
        // ScreenTemp
        ui_btn_temp_nozzle, ui_btn_temp_bed, ui_btn_temp_preheat,
        // ScreenSetTemp
        ui_img_set_temp_dial, ui_btn_set_temp_ok,
        // ScreenSetExtrude
        ui_btn_set_extrude_ok,
        // ScreenRoller
        ui_img_roller_left, ui_img_roller_right,
        // ScreenBacklight
        ui_img_backlight,
        // ScreenDialog
        ui_btn_dialog_ok,
        // ScreenPopup
        ui_img_popup, ui_btn_popup_ok,
        // ScreenColorWheel
        ui_btn_color_wheel_ok,
        // ScreenInfo
        ui_img_left_sta_ip, ui_img_mid_sta_ip, ui_img_right_sta_ip,
        ui_img_left_ap_ip, ui_img_mid_ap_ip, ui_img_right_ap_ip,
        ui_img_left_local, ui_img_mid_local, ui_img_right_local,
        ui_img_left_host, ui_img_mid_host, ui_img_right_host,
        // ScreenTestSensor
        ui_btn_test_wifi,
    };
    for (uint16_t i = 0; i < ACOUNT(img_obj); i++) {
        lv_obj_set_style_img_recolor(img_obj[i], lv_theme_color(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(img_obj[i], opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    lv_obj_t * const bg_img_obj[] = {
        // ScreenSetExtrude
        ui_roller_set_extrude_length, ui_roller_set_extrude_speed,
        // ScreenRoller
        ui_roller,
    };
    for (uint16_t i = 0; i < ACOUNT(bg_img_obj); i++) {
        lv_obj_set_style_bg_img_recolor(bg_img_obj[i], lv_theme_color(), LV_PART_SELECTED | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_recolor_opa(bg_img_obj[i], opa, LV_PART_SELECTED | LV_STATE_DEFAULT);
    }

    lv_obj_t * const indicator_img_obj[] = {
        ui_slider_heating_nozzle,
        // ScreenHeatingBed
        ui_slider_heating_bed,
        // ScreenBacklight
        ui_slider_backlight,
    };
    for (uint16_t i = 0; i < ACOUNT(indicator_img_obj); i++) {
        lv_obj_set_style_bg_img_recolor(indicator_img_obj[i], lv_theme_color(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_img_recolor_opa(indicator_img_obj[i], opa, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }

    lv_obj_t * const arc_obj[] = {
        // ScreenPrinting
        ui_arc_printing_progress, ui_arc_bg_1, ui_arc_bg_2_1, ui_arc_bg_2_2, ui_arc_bg_2_3,
    };
    for (uint16_t i = 0; i < ACOUNT(arc_obj); i++) {
        lv_obj_set_style_arc_color(arc_obj[i], lv_theme_color(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    }
}
