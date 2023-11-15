#include <stdio.h> // sprintf
#include "ui/ui.h"
#include "moonraker.h"
#include "lvgl_usr.h"

enum UI_TEMP_TYPE {
    UI_TEMP_NOZZLE = 0x00,
    UI_TEMP_BED,
    UI_TEMP_NULL,
};

UI_TEMP_TYPE lv_set_temp_type = UI_TEMP_NULL;
static lv_obj_t * previous_menu;
int16_t backend_target = 0, last_backend_target = 0,
        frontend_target = 0, last_frontend_target = 0;

// Screen Temp button call back
void lv_temp_set_type_nozzle(lv_event_t * e) {
    previous_menu = lv_scr_act();
    lv_set_temp_type = UI_TEMP_NOZZLE;
}

void lv_temp_set_type_bed(lv_event_t * e) {
    previous_menu = lv_scr_act();
    lv_set_temp_type = UI_TEMP_BED;
}

// back to previous menu
void lv_set_temp_back_to_previous(lv_event_t * e) {
    if (previous_menu) {
        lv_set_temp_type = UI_TEMP_NULL;
        _ui_screen_change(&previous_menu, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
        previous_menu = NULL;
        last_backend_target = last_frontend_target;
    }
}

// button
void lv_set_temp_btn_ok(lv_event_t * e) {
    char string_buffer[10];
    const char * gcode = (lv_set_temp_type == UI_TEMP_NOZZLE) ? "M104" : "M140";
    snprintf(string_buffer, sizeof(string_buffer), "%s S%d", gcode, last_frontend_target);
    moonraker.post_gcode_to_queue(string_buffer);
    lv_set_temp_back_to_previous(e);
}

typedef enum {
    KNOB_IDLE = 0,
    KNOB_INC = 1,
    KNOB_DEC,
} knob_event_t;

knob_event_t knob_get_event(const lv_area_t * knob_area, lv_indev_state_t state, int16_t x, int16_t y)
{
    #define TOUCH_DIFF 10
    knob_event_t knob_event = KNOB_IDLE;
    static uint16_t oldx = 0, oldy = 0;
    static uint8_t released = 0;

    if (state == LV_INDEV_STATE_REL) {
        released = 1;
        return KNOB_IDLE;
    }
    if (released) {
        released = 0;
        oldx = x;
        oldy = y;
        return KNOB_IDLE;
    }

    if (x > knob_area->x1 && x < knob_area->x2
     && y > knob_area->y1 && y < knob_area->y2) {
        if (x > oldx + TOUCH_DIFF) {
            if (oldy > (knob_area->y1 + knob_area->y2) / 2) {
                knob_event = KNOB_DEC;
            } else {
                knob_event = KNOB_INC;
            }
        } else if (x < oldx - TOUCH_DIFF) {
            if (oldy > (knob_area->y1 + knob_area->y2) / 2) {
                knob_event = KNOB_INC;
            } else {
                knob_event = KNOB_DEC;
            }
        }
        if (y > oldy + TOUCH_DIFF) {
            if (x > (knob_area->x1 + knob_area->x2) / 2) {
                knob_event = KNOB_INC;
            } else {
                knob_event = KNOB_DEC;
            }
        } else if (y < oldy - TOUCH_DIFF) {
            if (x > (knob_area->x1 + knob_area->x2) / 2) {
                knob_event = KNOB_DEC;
            } else {
                knob_event = KNOB_INC;
            }
        }
    }

    if (knob_event != KNOB_IDLE) {
        oldx = x;
        oldy = y;
    }

    return knob_event;
}

#include <Arduino.h>
// set temp screen rotate event
void lv_loop_set_temp_screen(void) {
    // only need to handle on set temp screen
    if (lv_scr_act() != ui_ScreenSetTemp) return;

    if (lv_set_temp_type == UI_TEMP_BED) {
        backend_target = moonraker.data.bed_target;
    } else if (lv_set_temp_type == UI_TEMP_NOZZLE) {
        backend_target = moonraker.data.nozzle_target;
    }

    if (last_backend_target != backend_target) {
        frontend_target = last_backend_target = backend_target;
    }

    if (last_frontend_target != frontend_target) {
        char string_buffer[8];
        last_frontend_target = frontend_target;
        snprintf(string_buffer, sizeof(string_buffer), "%d℃", last_frontend_target);
        lv_label_set_text(ui_set_temp_target, string_buffer);
    }

    const lv_area_t knob_area = {
        0, 0, 240, 240
    };
    static int16_t angle = 0;
    extern lv_indev_t * ts_cst816s_indev;
    lv_indev_data_t data;
    _lv_indev_read(ts_cst816s_indev, &data);

    knob_event_t event = knob_get_event(&knob_area, data.state, data.point.x, data.point.y);
    if (event != KNOB_IDLE) {
        #define DIAL_ANGLE_INC 30
        #define TARGET_INC 5
        if (event == KNOB_INC) {
            frontend_target += TARGET_INC;
            lv_img_set_angle(ui_img_set_temp_dial, angle += DIAL_ANGLE_INC);
        } else if (event == KNOB_DEC) {
            frontend_target -= TARGET_INC;
            lv_img_set_angle(ui_img_set_temp_dial, angle -= DIAL_ANGLE_INC);
        }
    }
}
