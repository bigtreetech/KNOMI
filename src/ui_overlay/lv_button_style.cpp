#include "ui/ui.h"
#include "knomi.h"
#include "lv_overlay.h"

// Add a transition to the outline
static lv_style_prop_t btn_style_props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_INV};
static lv_style_transition_dsc_t btn_style_trans = {
    .props = btn_style_props,
    .path_xcb = lv_anim_path_linear,
    .time = 300,
    .delay = 0,
};

void lv_btn_set_style(lv_obj_t * const obj, lv_coord_t radius, lv_color_t outline_color) {
    lv_obj_set_style_radius(obj, radius, LV_STATE_PRESSED);
    // move down 5 pixel when pressed
    lv_obj_set_style_translate_y(obj, 5, LV_STATE_PRESSED);
    // Add a large outline when pressed
    lv_obj_set_style_outline_color(obj, outline_color, LV_STATE_PRESSED);
    lv_obj_set_style_outline_width(obj, 30, LV_STATE_PRESSED);
    lv_obj_set_style_outline_opa(obj, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_set_style_transition(obj, &btn_style_trans, LV_STATE_PRESSED);
}
/**
 * Style for img button
 */
void lv_btn_add_style(void) {
    // circle button
    lv_obj_t * const cir_btn_obj[] = {
        // Common button (ok)
        ui_btn_set_extrude_ok, ui_btn_set_temp_ok, ui_btn_dialog_ok, ui_btn_popup_ok,
        // ScreenMove
        ui_btn_move_abl, ui_btn_move_qgl, ui_btn_move_home,
        // ScreenColorWheel
        ui_btn_color_wheel_ok,
    };
    for (uint8_t i = 0; i < ACOUNT(cir_btn_obj); i++) {
        lv_btn_set_style(cir_btn_obj[i], 30, lv_theme_color());
    }
    // circle cancle button, 0x3B3B3B is icon color
    lv_btn_set_style(ui_btn_dialog_cancel, 30, lv_color_hex(0x3B3B3B));

    // rectangle button
    lv_obj_t * const rec_btn_obj[] = {
        // ScreenExtrude
        ui_btn_extruder_retract, ui_btn_extruder_extrude, ui_btn_extruder_temp, ui_btn_extruder_speed,
        // ScreenTemp
        ui_btn_temp_nozzle, ui_btn_temp_bed, ui_btn_temp_preheat,
        // ScreenRoller
        ui_btn_roller,
        // ScreenTestSensor
        ui_btn_test_wifi,
    };
    for (uint8_t i = 0; i < ACOUNT(rec_btn_obj); i++) {
        lv_btn_set_style(rec_btn_obj[i], 3, lv_theme_color());
    }
}
