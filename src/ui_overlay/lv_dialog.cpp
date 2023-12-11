#include "ui/ui.h"
#include "knomi.h"
#include "moonraker.h"

enum UI_DIALOG_TYPE {
    UI_DIALOG_ABL = 0x00,
    UI_DIALOG_QGL,
    UI_DIALOG_HOME,
    UI_DIALOG_PRINT,
    UI_DIALOG_CANCEL_PRINT,
    UI_DIALOG_PAUSE_PRINT,
    UI_DIALOG_RESUME_PRINT,
    UI_DIALOG_FACTORY_RESET,
    UI_DIALOG_MOONRAKER_CUSTOM,
};

static String moonraker_custom_cmd;

static lv_obj_t * dialog_previous_menu;

static const char *dialog_title[] = {
    "Start ABL?",
    "Start QGL?",
    "Start Home?",
    "Start Printing?",
    "Cancel Printing?",
    "Pause Printing?",
    "Resume Printing?",
    "Factory Reset?",
};

static UI_DIALOG_TYPE dialog_type;

static void lv_dialog_goto(UI_DIALOG_TYPE type) {
    dialog_type = type;
    dialog_previous_menu = lv_scr_act();
    if (dialog_type != UI_DIALOG_MOONRAKER_CUSTOM) {
        lv_label_set_text(ui_label_dialog, dialog_title[dialog_type]);
    }
    _ui_screen_change(&ui_ScreenDialog, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
}

void lv_dialog_set_custom(const char * text, String cmd) {
    lv_label_set_text(ui_label_dialog, text);
    moonraker_custom_cmd = cmd;
    lv_dialog_goto(UI_DIALOG_MOONRAKER_CUSTOM);
}


void lv_dialog_goto_abl(lv_event_t * e) {
    lv_dialog_goto(UI_DIALOG_ABL);
}

void lv_dialog_goto_qgl(lv_event_t * e) {
    lv_dialog_goto(UI_DIALOG_QGL);
}

void lv_dialog_goto_home(lv_event_t * e) {
    lv_dialog_goto(UI_DIALOG_HOME);
}

void lv_dialog_goto_cancel(lv_event_t * e) {
    lv_dialog_goto(UI_DIALOG_CANCEL_PRINT);
}

void lv_dialog_goto_pause_resume(lv_event_t * e) {
    if (moonraker.data.pause) {
        lv_dialog_goto(UI_DIALOG_RESUME_PRINT);
    } else {
        lv_dialog_goto(UI_DIALOG_PAUSE_PRINT);
    }
}

void lv_dialog_goto_print(void) {
    lv_dialog_goto(UI_DIALOG_PRINT);
}

void lv_dialog_goto_reset_wifi(void) {
    lv_dialog_goto(UI_DIALOG_FACTORY_RESET);
}

// back to previous menu
void lv_dialog_back_to_previous_menu(lv_event_t * e) {
    if (dialog_previous_menu) {
        _ui_screen_change(&dialog_previous_menu, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
    }
}

// button
void lv_dialog_btn_ok(lv_event_t * e) {
    switch (dialog_type) {
        case UI_DIALOG_ABL:
            moonraker.post_gcode_to_queue("BED_MESH_CALIBRATE");
            break;
        case UI_DIALOG_QGL:
            moonraker.post_gcode_to_queue("QUAD_GANTRY_LEVEL");
            break;
        case UI_DIALOG_HOME:
            moonraker.post_gcode_to_queue("G28");
            break;
        case UI_DIALOG_PRINT: {
                char gcode[1024];
                lv_roller_get_selected_str(ui_roller, gcode, sizeof(gcode));
                String path = gcode;
                moonraker.post_to_queue("/printer/print/start?filename=" + path);
            }
            break;
        case UI_DIALOG_CANCEL_PRINT:
            moonraker.post_to_queue("/printer/print/cancel");
            break;
        case UI_DIALOG_PAUSE_PRINT:
            moonraker.post_to_queue("/printer/print/pause");
            break;
        case UI_DIALOG_RESUME_PRINT:
            moonraker.post_to_queue("/printer/print/resume");
            break;
        case UI_DIALOG_FACTORY_RESET:
            knomi_factory_reset();
            break;
        case UI_DIALOG_MOONRAKER_CUSTOM:
            moonraker.post_to_queue(moonraker_custom_cmd);
            break;
    }

    lv_dialog_back_to_previous_menu(e);
}
