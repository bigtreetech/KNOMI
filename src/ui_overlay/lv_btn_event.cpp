#include "ui/ui.h"
#include "knomi.h"
#include "lv_overlay.h"
#include "pinout.h"
#include "button.h"

void lv_btn_init(void) {
    Button_Init(BOOT_PIN);
}

void lv_test_wifi_ssid_update(void) {
    lv_label_set_text(ui_label_ssid1, wifi_scan.ssid[0]);
    lv_label_set_text(ui_label_ssid2, wifi_scan.ssid[1]);
    lv_label_set_text(ui_label_ssid3, wifi_scan.ssid[2]);
}

void lv_test_sensor_btn_ok(lv_event_t * e) {
    lv_label_set_text(ui_label_ssid1, " ");
    lv_label_set_text(ui_label_ssid2, " ");
    lv_label_set_text(ui_label_ssid3, " ");
    knomi_config_require_change(WEB_POST_WIFI_REFRESH);
    wifi_scan_refresh_set_callback(lv_test_wifi_ssid_update);
}

void lv_loop_btn_event(void) {
    v_btn_t btn = Button_GetAction(BOOT_PIN);

    if (btn != BTN_IDLE) {
        Serial.print("btn: ");
        Serial.println(btn, HEX);
    }

    if (btn == BTN_LONG_CLICK) {
        knomi_factory_reset();
    }

    if (btn == BTN_TRIPLE_CLICK && lv_scr_act() != ui_ScreenTestImg && lv_scr_act() != ui_ScreenTestSensor) {
        _ui_screen_change(&ui_ScreenTestImg, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
    }

#ifndef CST816S_SUPPORT
    if (btn == BTN_TRIPLE_CLICK) {
        if (lv_scr_act() == ui_ScreenTestImg){
            _ui_screen_change(&ui_ScreenTestSensor, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0, NULL);
        }
        if (lv_scr_act() == ui_ScreenTestSensor){
            _ui_screen_change(&ui_ScreenTestImg, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0, NULL);
        }
    }
    if (btn == BTN_CLICK) {
        lv_test_sensor_btn_ok(NULL);
    }
#endif

#ifdef LIS2DW_SUPPORT
    if(lv_scr_act() == ui_ScreenTestSensor) {
        // accelerometer
        lv_slider_set_value(ui_slider_acc_x, abs(lis2dw12_acc[0]) / 10, LV_ANIM_ON);
        lv_slider_set_value(ui_slider_acc_y, abs(lis2dw12_acc[1]) / 10, LV_ANIM_ON);
        lv_slider_set_value(ui_slider_acc_z, abs(lis2dw12_acc[2]) / 10, LV_ANIM_ON);

        char buf[10];
        snprintf(buf, sizeof(buf), "%d", lis2dw12_acc[0] / 100);
        lv_label_set_text(ui_label_test_x, buf);
        snprintf(buf, sizeof(buf), "%d", lis2dw12_acc[1] / 100);
        lv_label_set_text(ui_label_test_y, buf);
        snprintf(buf, sizeof(buf), "%d", lis2dw12_acc[2] / 100);
        lv_label_set_text(ui_label_test_z, buf);
    }
#endif
}
// WiFi.scanNetworks(true, false, true, 75U);
