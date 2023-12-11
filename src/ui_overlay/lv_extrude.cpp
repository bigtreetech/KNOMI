#include "stdio.h" // sprintf
#include "ui/ui.h"
#include "knomi.h"
#include "moonraker.h"

/*!
 *  @param  symbol
 *          "+": extrude
 *          "-": retract
 */
void extruder_action(const char* symbol) {
    uint16_t len_id = lv_roller_get_selected(ui_roller_set_extrude_length);
    uint16_t speed_id = lv_roller_get_selected(ui_roller_set_extrude_speed);
    const int16_t length[] = {
        EXTRUDE_MM_0, EXTRUDE_MM_1, EXTRUDE_MM_2, EXTRUDE_MM_3, EXTRUDE_MM_4
    };
    const int16_t speed[] = {
        EXTRUDE_MM_S_0, EXTRUDE_MM_S_1, EXTRUDE_MM_S_2, EXTRUDE_MM_S_3, EXTRUDE_MM_S_4
    };
    moonraker.post_gcode_to_queue("M83");
    char buf[100];
    sprintf(buf, "G1 E%s%d F%d", symbol, length[len_id], MMS_TO_MMM(speed[speed_id]));
    moonraker.post_gcode_to_queue(buf);
}

void lv_btn_extruder_extrude(lv_event_t * e) {
    extruder_action("+");
}
void lv_btn_extruder_retract(lv_event_t * e) {
    extruder_action("-");
}
