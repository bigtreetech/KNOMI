#include "ui/ui.h"
#include "knomi.h"
#include "moonraker.h"
#include "lv_overlay.h"

typedef enum {
    LV_MOONRAKER_STATE_IDLE = 0,
    LV_MOONRAKER_STATE_HOMING,
    LV_MOONRAKER_STATE_PROBING,
    LV_MOONRAKER_STATE_QGLING,
    LV_MOONRAKER_STATE_NOZZLE_HEATING,
    LV_MOONRAKER_STATE_BED_HEATING,
    LV_MOONRAKER_STATE_PRINTING,
    LV_SCREEN_STATE_INIT,
    LV_SCREEN_STATE_IDLE,
    LV_SCREEN_HEATED,
    LV_SCREEN_PRINT,
    LV_SCREEN_PRINT_OK,
    LV_SCREEN_PRINTED,
    LV_SCREEN_STATE_PLAYING,
} lv_screen_state_t;

lv_obj_t * ui_img_main_gif;
const lv_img_dsc_t * gif_idle[] = {&gif_zerog, &gif_standby};

static char string_buffer[8];
static lv_screen_state_t lv_screen_state = LV_MOONRAKER_STATE_IDLE;
static lv_obj_t * ui_ScreenIdle = NULL;
static lv_obj_t * ui_ScreenNow = NULL;

static void lv_goto_busy_screen(lv_obj_t * screen, lv_screen_state_t state, const lv_img_dsc_t * gif) {
    if (lv_screen_state == state) return;
    lv_screen_state = state;

    //
    if (gif) lv_gif_set_src(ui_img_main_gif, gif);
    lv_obj_clear_flag(ui_ScreenMainGif, LV_OBJ_FLAG_CLICKABLE);

    // backup now screen, cause lv_scr_act() is delayed updates
    if (ui_ScreenNow == NULL) ui_ScreenNow = lv_scr_act();
    if (screen && screen != ui_ScreenNow) {
        // backup screen before jump
        if (ui_ScreenIdle == NULL) ui_ScreenIdle = ui_ScreenNow;
        ui_ScreenNow = screen;
        _ui_screen_change(&screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0, NULL);
    }
}

static void lv_goto_idle_screen(void) {
    if (lv_screen_state == LV_MOONRAKER_STATE_IDLE) return;
    lv_screen_state = LV_MOONRAKER_STATE_IDLE;

    //
    lv_gif_set_src(ui_img_main_gif, gif_idle[0]);
    lv_obj_add_flag(ui_ScreenMainGif, LV_OBJ_FLAG_CLICKABLE);

    // goto the screen backed up before
    if (ui_ScreenIdle && (ui_ScreenIdle != lv_scr_act())) {
        _ui_screen_change(&ui_ScreenIdle, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, NULL);
    }
    ui_ScreenIdle = NULL;
    ui_ScreenNow = NULL;
}


#define TEMPERATURE_ERROR_RANGE 2

bool moonraker_nozzle_is_heating(void) {
    if (moonraker.data.heating_nozzle)
        return true;
    if (moonraker.data.nozzle_actual + TEMPERATURE_ERROR_RANGE < moonraker.data.nozzle_target)
        return true;
    if ((moonraker.data.nozzle_target != 0) && (moonraker.data.nozzle_target + TEMPERATURE_ERROR_RANGE < moonraker.data.nozzle_actual))
        return true;
    return false;
}

bool moonraker_bed_is_heating(void) {
    if (moonraker.data.heating_bed)
        return true;
    if (moonraker.data.bed_actual + TEMPERATURE_ERROR_RANGE < moonraker.data.bed_target)
        return true;
    if ((moonraker.data.bed_target != 0) && (moonraker.data.bed_target + TEMPERATURE_ERROR_RANGE < moonraker.data.bed_actual))
        return true;
    return false;
}

// screen change according to moonraker status
void lv_loop_moonraker_change_screen(void) {

    static lv_screen_state_t screen_state = LV_SCREEN_STATE_INIT;
    // if (moonraker.data.printing) {
    //     if (lv_screen_state == 0) {
    //         lv_goto_busy_screen(ui_ScreenPrinting, LV_MOONRAKER_STATE_PRINTING, NULL);
    //         return;
    //     }
    // }
    if (moonraker.data.homing) {
        lv_goto_busy_screen(ui_ScreenMainGif, LV_MOONRAKER_STATE_HOMING, &gif_homing);
        return;
    }
    if (moonraker.data.probing) {
        lv_goto_busy_screen(ui_ScreenMainGif, LV_MOONRAKER_STATE_PROBING, &gif_probing);
        return;
    }
    if (moonraker.data.qgling) {
        lv_goto_busy_screen(ui_ScreenMainGif, LV_MOONRAKER_STATE_QGLING, &gif_qgling);
        return;
    }
    if (moonraker_nozzle_is_heating()) {
        lv_goto_busy_screen(ui_ScreenHeatingNozzle, LV_MOONRAKER_STATE_NOZZLE_HEATING, NULL);
        if (moonraker.data.printing)
            screen_state = LV_SCREEN_HEATED;
        return;
    }
    if (moonraker_bed_is_heating()) {
        lv_goto_busy_screen(ui_ScreenHeatingBed, LV_MOONRAKER_STATE_BED_HEATING, NULL);
        if (moonraker.data.printing)
            screen_state = LV_SCREEN_HEATED;
        return;
    }

    static lv_screen_state_t playing_next_state = LV_SCREEN_STATE_INIT;
    static uint32_t playing_ms = 0;
    static lv_screen_state_t playing_state;
    static const lv_img_dsc_t * playing_img;
    switch (screen_state) {
        case LV_SCREEN_STATE_INIT:
            if (moonraker.data.printing) {
                if (moonraker.data.bed_actual + TEMPERATURE_ERROR_RANGE > moonraker.data.bed_target &&
                  moonraker.data.nozzle_actual + TEMPERATURE_ERROR_RANGE > moonraker.data.nozzle_target) {
                    screen_state = LV_SCREEN_HEATED;
                    return;
                }
            }
            break;
        case LV_SCREEN_STATE_IDLE:
            if (!moonraker.data.printing) {
                screen_state = LV_SCREEN_PRINT_OK;
                return;
            }
            break;
        case LV_SCREEN_HEATED:
            playing_state = LV_SCREEN_HEATED;
            playing_img = &gif_heated;
            screen_state = LV_SCREEN_STATE_PLAYING;
            playing_next_state = LV_SCREEN_PRINT;
            playing_ms = millis() + 7000;
            return;
        case LV_SCREEN_PRINT:
            lv_goto_busy_screen(ui_ScreenMainGif, LV_SCREEN_PRINT, &gif_print);
            if (moonraker.data.progress >= 1 || !moonraker.data.printing)
                screen_state = LV_SCREEN_STATE_IDLE;
            return;
        case LV_SCREEN_STATE_PLAYING:
            if (playing_ms > millis()) {
                lv_goto_busy_screen(ui_ScreenMainGif, playing_state, playing_img);
                return;
            }
            screen_state = playing_next_state;
            return;
        case LV_SCREEN_PRINT_OK:
            playing_state = LV_SCREEN_PRINT_OK;
            playing_img = &gif_print_ok;
            screen_state = LV_SCREEN_STATE_PLAYING;
            playing_next_state = LV_SCREEN_PRINTED;
            playing_ms = millis() + 1600;
            return;
        case LV_SCREEN_PRINTED:
            playing_state = LV_SCREEN_PRINTED;
            playing_img = &gif_printed;
            screen_state = LV_SCREEN_STATE_PLAYING;
            playing_next_state = LV_SCREEN_STATE_INIT;
            playing_ms = millis() + 7000;
            return;
    }

    // Printing must be lastest, the lowest priority
    // That The status screen(homing, heating, etc.) can occupy this screen
    if (moonraker.data.printing) {
        lv_goto_busy_screen(ui_ScreenPrinting, LV_MOONRAKER_STATE_PRINTING, NULL);
        return;
    }
    // back to previous screen
    lv_goto_idle_screen();

    if (lv_scr_act() == ui_ScreenMainGif) {
        static uint8_t gif_idle_index = 0;
        static uint32_t gif_idle_ms = 0;

        if (gif_idle_ms < millis()) {
            lv_gif_set_src(ui_img_main_gif, gif_idle[gif_idle_index]);
            gif_idle_index = (gif_idle_index + 1) % ACOUNT(gif_idle);
            gif_idle_ms = millis() + 7000; // 7s
        }
    }
}

// modify all temperature label
void lv_loop_moonraker_change_screen_value(void) {
    // nozzle target
    static int16_t nozzle_target;
    if (nozzle_target != moonraker.data.nozzle_target) {
        nozzle_target = moonraker.data.nozzle_target;
        snprintf(string_buffer, sizeof(string_buffer), "%d℃", nozzle_target);
        lv_label_set_text(ui_label_extruder_target, string_buffer);
        lv_label_set_text(ui_label_temp_nozzle_target, string_buffer);
        lv_label_set_text(ui_label_heating_nozzle_target, string_buffer);
    }
    // nozzle actual
    static int16_t nozzle_actual;
    if (nozzle_actual != moonraker.data.nozzle_actual) {
        nozzle_actual = moonraker.data.nozzle_actual;
        snprintf(string_buffer, sizeof(string_buffer), "%d℃", nozzle_actual);
        lv_label_set_text(ui_label_extruder_actual, string_buffer);
        lv_label_set_text(ui_label_temp_nozzle_actual, string_buffer);
        lv_label_set_text(ui_label_heating_nozzle_actual, string_buffer);
    }
    // bed target
    static int16_t bed_target;
    if (bed_target != moonraker.data.bed_target) {
        bed_target = moonraker.data.bed_target;
        snprintf(string_buffer, sizeof(string_buffer), "%d℃", bed_target);
        lv_label_set_text(ui_label_temp_bed_target, string_buffer);
        lv_label_set_text(ui_label_heating_bed_target, string_buffer);
    }
    // bed actual
    static int16_t bed_actual;
    if (bed_actual != moonraker.data.bed_actual) {
        bed_actual = moonraker.data.bed_actual;
        snprintf(string_buffer, sizeof(string_buffer), "%d℃", bed_actual);
        lv_label_set_text(ui_label_temp_bed_actual, string_buffer);
        lv_label_set_text(ui_label_heating_bed_actual, string_buffer);
    }
    // progress
    static uint8_t progress;
    if (progress != moonraker.data.progress) {
        progress = moonraker.data.progress;
        lv_arc_set_value(ui_arc_printing_progress, progress);
        snprintf(string_buffer, sizeof(string_buffer), "%d%%", progress);
        lv_label_set_text(ui_label_printing_progress, string_buffer);
    }
#ifdef LIS2DW_SUPPORT
    // accelerometer
    lv_slider_set_value(ui_slider_printing_acc_x, abs(lis2dw12_acc[0]) / 10, LV_ANIM_ON);
    lv_slider_set_value(ui_slider_printing_acc_y, abs(lis2dw12_acc[1]) / 10, LV_ANIM_ON);
    lv_slider_set_value(ui_slider_printing_acc_z, abs(lis2dw12_acc[2] + 980) / 10, LV_ANIM_ON); // +980 for counteract the value of gravitational acceleration
#endif

    if ((moonraker.data.nozzle_target != 0) && (lv_scr_act() == ui_ScreenHeatingNozzle)) {
        lv_slider_set_value(ui_slider_heating_nozzle, moonraker.data.nozzle_actual * 100 / moonraker.data.nozzle_target, LV_ANIM_ON);
    }
    if ((moonraker.data.bed_target != 0) && (lv_scr_act() == ui_ScreenHeatingBed)) {
        lv_slider_set_value(ui_slider_heating_bed, moonraker.data.bed_actual * 100 / moonraker.data.bed_target, LV_ANIM_ON);
    }
}
