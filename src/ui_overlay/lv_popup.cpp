#include "ui/ui.h"
#include "knomi.h"
#include "moonraker.h"

typedef enum {
    LV_POPUP_NULL = 0,
    LV_POPUP_UNCONNECT,
    LV_POPUP_PRINTER_ERR,
    LV_POPUP_ACTION_ERR,
} lv_popup_status_t;

static lv_popup_status_t lv_popup_status = LV_POPUP_NULL;
static lv_obj_t * previous_menu;

void lv_popup_warning(const char * warning, bool clickable) {
    lv_textarea_set_text(ui_textarea_popup, warning);

    if (clickable) {
        lv_obj_clear_flag(ui_btn_popup_ok, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(ui_img_popup, -15);
    } else {
        lv_obj_add_flag(ui_btn_popup_ok, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(ui_img_popup, 0);
    }

    if (lv_scr_act() != ui_ScreenPopup) {
        previous_menu = lv_scr_act();
        _ui_screen_change(&ui_ScreenPopup, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
    }
}

static void lv_popup_remove(lv_event_t * e) {
    if (previous_menu && (previous_menu != lv_scr_act())) {
        _ui_screen_change(&previous_menu, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
    }
    previous_menu = NULL;
}

void ui_event_popup_ok(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_popup_remove(e);
    }
}

static void lv_goto_popup_screen(lv_popup_status_t state, const char * warning) {
    if (lv_popup_status == state) return;
    lv_popup_status = state;
    lv_popup_warning(warning, false);
}

static void lv_remove_popup_screen(void) {
    if (lv_popup_status == LV_POPUP_NULL) return;
    lv_popup_status = LV_POPUP_NULL;
    lv_popup_remove(NULL);
}

void lv_loop_popup_screen(void) {
    if (moonraker.unconnected) {
        lv_goto_popup_screen(LV_POPUP_UNCONNECT, "Moonraker Connect failed\nPlease check Your printer or KNOMI IP");
        return;
    }
    if (moonraker.unready) {
        lv_goto_popup_screen(LV_POPUP_PRINTER_ERR, "Printer is Unoperational\nPlease check your Printer");
        return;
    }
    lv_remove_popup_screen();
}
