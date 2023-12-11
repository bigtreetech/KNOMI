#include "ui/ui.h"
#include "knomi.h"

static wifi_status_t lv_wifi_status = WIFI_STATUS_INIT;

static void lv_goto_wifi_screen(wifi_status_t state) {
    if (lv_wifi_status == state) return;

    static lv_obj_t * running_menu = ui_ScreenMainGif;
    if (lv_wifi_status == WIFI_STATUS_CONNECTED) {
        running_menu = lv_scr_act();
    }
    lv_obj_t * target = NULL;
    switch (state) {
        case WIFI_STATUS_ERROR:
            target = ui_ScreenWelcome;
            break;
        case WIFI_STATUS_CONNECTING:
            target = ui_ScreenWIFIConnecting;
            break;
        case WIFI_STATUS_CONNECTED:
            target = running_menu;
            break;
        case WIFI_STATUS_DISCONNECT:
            target = ui_ScreenWIFIDisconnect;
            break;
    }
    lv_wifi_status = state;
    Serial.print("lv_wifi_status: ");
    Serial.println(lv_wifi_status);

    _ui_screen_change(&target, LV_SCR_LOAD_ANIM_NONE, 0, 0, NULL);
}

void lv_loop_wifi_change_screen(wifi_status_t status) {
    lv_goto_wifi_screen(status);
}
