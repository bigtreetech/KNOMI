#include "ui/ui.h"
#include "knomi.h"
#include "moonraker.h"
#include <ArduinoJson.h>
#include "lv_overlay.h"

enum UI_ROLLER_TYPE {
    UI_ROLLER_SETTING = 0x00,
    UI_ROLLER_SETTING_THEME,
    UI_ROLLER_CONTROL_KLIPPER,
    UI_ROLLER_SERVICE,
    UI_ROLLER_CONTROL_SERVICE,
    UI_ROLLER_CONTROL_HOST,
    UI_ROLLER_PREHEAT,
    UI_ROLLER_PRINT,
    UI_ROLLER_MENU_NUM,

    UI_ROLLER_NULL = 0xff,
};

typedef struct {
    const char *options;
    uint16_t sel_opt;
    UI_ROLLER_TYPE this_type;
    lv_obj_t ** previous_menu;
    UI_ROLLER_TYPE previous_type;
}lv_roller_menu_t;

char gcode_options[1024];
char service_options[1024];
static lv_roller_menu_t roller_menu[UI_ROLLER_MENU_NUM] = {
    {
        .options = "UI color\nBacklight\nKlipper Control\nService Control\nHost Control\nKnomi Info\nFactory Reset",
        .sel_opt = 0,
        .this_type = UI_ROLLER_SETTING,
        .previous_menu = &ui_ScreenTemp,
        .previous_type = UI_ROLLER_NULL,
    },
    {
        .options = "Default\nBlue\nPurple\nGreen\nCustom",
        .sel_opt = 0,
        .this_type = UI_ROLLER_SETTING_THEME,
        .previous_menu = NULL,
        .previous_type = UI_ROLLER_SETTING,
    },
    {
        .options = "Restart\nFirmware Restart",
        .sel_opt = 0,
        .this_type = UI_ROLLER_CONTROL_KLIPPER,
        .previous_menu = NULL,
        .previous_type = UI_ROLLER_SETTING,
    },
    {
        .options = service_options,
        .sel_opt = 0,
        .this_type = UI_ROLLER_SERVICE,
        .previous_menu = NULL,
        .previous_type = UI_ROLLER_SETTING,
    },
    {
        .options = "Start\nStop\nRestart",
        .sel_opt = 0,
        .this_type = UI_ROLLER_CONTROL_SERVICE,
        .previous_menu = NULL,
        .previous_type = UI_ROLLER_SERVICE,
    },
    {
        .options = "Reboot\nShutdown",
        .sel_opt = 0,
        .this_type = UI_ROLLER_CONTROL_HOST,
        .previous_menu = NULL,
        .previous_type = UI_ROLLER_SETTING,
    },
    {
        .options = PREHEAT_0_LABEL "\n"\
                   PREHEAT_1_LABEL "\n"\
                   PREHEAT_2_LABEL "\n"\
                   PREHEAT_3_LABEL "\n"\
                   PREHEAT_4_LABEL "\n"\
                   PREHEAT_5_LABEL "\n"\
                   PREHEAT_6_LABEL,
        .sel_opt = 0,
        .this_type = UI_ROLLER_PREHEAT,
        .previous_menu = &ui_ScreenTemp,
        .previous_type = UI_ROLLER_NULL,
    },
    {
        .options = gcode_options, // NULL, // Dynamic gcode
        .sel_opt = 0,
        .this_type = UI_ROLLER_PRINT,
        .previous_menu = &ui_ScreenMainGif,
        .previous_type = UI_ROLLER_NULL,
    },
};

lv_roller_menu_t * cur_menu;

void lv_roller_set_type(UI_ROLLER_TYPE type) {
    for (uint8_t i = 0; i < ACOUNT(roller_menu); i++) {
        lv_roller_menu_t *cur = roller_menu + i;
        if (type == cur->this_type) {
            cur_menu = cur;
            lv_roller_set_options(ui_roller, cur_menu->options, LV_ROLLER_MODE_NORMAL);
            lv_roller_set_selected(ui_roller, cur_menu->sel_opt, LV_ANIM_ON);
        }
    }
}

void lv_roller_set_type_settings(lv_event_t * e) {
    lv_roller_set_type(UI_ROLLER_SETTING);
}


void lv_roller_set_type_print(lv_event_t * e) {
    String list = moonraker.send_request("GET", "/server/files/list?");
    if (!list.isEmpty()) {
        DynamicJsonDocument json_parse(list.length() * 2);
        deserializeJson(json_parse, list);
        JsonArray files = json_parse["result"].as<JsonArray>();
        String gcodes;
        for (JsonObject file : files) {
            gcodes += file["path"].as<String>() + "\n";
        }
        strlcpy(gcode_options, gcodes.c_str(), sizeof(gcode_options));
        gcode_options[min(sizeof(gcode_options), gcodes.length()) - 1] = 0;
    }

    lv_roller_set_type(UI_ROLLER_PRINT);
}

void lv_roller_set_type_preheat(lv_event_t * e) {
    lv_roller_set_type(UI_ROLLER_PREHEAT);
}

// back to previous menu
void lv_roller_back_to_previous_menu(lv_event_t * e) {
    if (cur_menu == NULL) return;
    // back to else menu
    if (cur_menu->previous_menu && *cur_menu->previous_menu) {
        _ui_screen_change(cur_menu->previous_menu, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
    }
    // back to else roller context
    if (cur_menu->previous_type != UI_ROLLER_NULL) {
        lv_roller_set_type(cur_menu->previous_type);
    }
}

const lv_color_t theme_colors[] = {
    lv_color_hex(LV_DEFAULT_COLOR), lv_color_hex(LV_32BIT_BTT_BLUE),
    lv_color_hex(LV_32BIT_BTT_PURPLE), lv_color_hex(LV_32BIT_BTT_GREEN)
};

uint16_t lv_roller_theme_color_id(void) {
    lv_color_t c = lv_theme_color();
    for (uint8_t i = 0; i < ACOUNT(theme_colors); i++) {
        if (c.full == theme_colors[i].full) {
            return i;
        }
    }
    return ACOUNT(theme_colors);
}

void lv_roller_theme_color_clicked(lv_event_t * e, uint16_t opt_id) {
    if (opt_id < ACOUNT(theme_colors)) {
        lv_theme_update_color(theme_colors[opt_id]);
        lv_roller_back_to_previous_menu(e);
    } else {
        _ui_screen_change(&ui_ScreenColorWheel, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
    }
}

void lv_colorwheel_value_change(lv_event_t * e) {
    lv_color_t c = lv_colorwheel_get_rgb(ui_theme_color_wheel);

    lv_obj_set_style_outline_color(ui_btn_color_wheel_ok, c, LV_STATE_PRESSED);
    lv_obj_set_style_img_recolor(ui_btn_color_wheel_ok, c, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(ui_btn_color_wheel_ok, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void lv_colorwheel_btn_ok(lv_event_t * e) {
    lv_color_t c = lv_colorwheel_get_rgb(ui_theme_color_wheel);

    lv_theme_update_color(c);
    _ui_screen_change(&ui_ScreenRoller, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
}

void lv_roller_preheat_clicked(lv_event_t * e, uint16_t opt_id) {
    const String nozzle[] = {
        STRINGIFY(PREHEAT_0_NOZZLE), STRINGIFY(PREHEAT_1_NOZZLE),
        STRINGIFY(PREHEAT_2_NOZZLE), STRINGIFY(PREHEAT_3_NOZZLE),
        STRINGIFY(PREHEAT_4_NOZZLE), STRINGIFY(PREHEAT_5_NOZZLE),
        STRINGIFY(PREHEAT_6_NOZZLE),
    };
    const String bed[] = {
        STRINGIFY(PREHEAT_0_BED), STRINGIFY(PREHEAT_1_BED),
        STRINGIFY(PREHEAT_2_BED), STRINGIFY(PREHEAT_3_BED),
        STRINGIFY(PREHEAT_4_BED), STRINGIFY(PREHEAT_5_BED),
        STRINGIFY(PREHEAT_6_BED),
    };
    moonraker.post_gcode_to_queue("M104 S" + nozzle[opt_id]);
    moonraker.post_gcode_to_queue("M140 S" + bed[opt_id]);

    lv_roller_back_to_previous_menu(e);
}

String service_name_id[20];
void lv_roller_set_service(void) {
    String list = moonraker.send_request("GET", "/machine/system_info");
    if (!list.isEmpty()) {
        DynamicJsonDocument json_parse(list.length() * 2);
        deserializeJson(json_parse, list);
        JsonArray files = json_parse["result"]["system_info"]["available_services"].as<JsonArray>();
        String services;
        uint8_t i = 0;
        for (JsonObject file : files) {
            String option = files[i].as<String>();
            service_name_id[i] = option;
            option[0] = toupper(option[0]);
            services += option + "\n";
            i++;
        }
        strlcpy(service_options, services.c_str(), sizeof(service_options));
        service_options[min(sizeof(service_options), services.length()) - 1] = 0;
    }

    lv_roller_set_type(UI_ROLLER_SERVICE);
}

void lv_roller_setting_clicked(lv_event_t * e, uint16_t opt_id) {
    switch (opt_id) {
        case 0:
            roller_menu[UI_ROLLER_SETTING_THEME].sel_opt = lv_roller_theme_color_id();
            lv_roller_set_type(UI_ROLLER_SETTING_THEME);
            break;
        case 1:
            _ui_screen_change(&ui_ScreenBacklight, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
            break;
        case 2:
            lv_roller_set_type(UI_ROLLER_CONTROL_KLIPPER);
            break;
        case 3:
            // lv_roller_set_type(UI_ROLLER_SERVICE);
            lv_roller_set_service();
            break;
        case 4:
            lv_roller_set_type(UI_ROLLER_CONTROL_HOST);
            break;
        case 5:
        {
            lv_label_set_text(ui_label_sta_ip, WiFi.localIP().toString().c_str());
            lv_label_set_text(ui_label_ap_ip, WiFi.softAPIP().toString().c_str());
            lv_label_set_text_fmt(ui_label_local, "%s.local", knomi_config.hostname);
            lv_label_set_text_fmt(ui_label_host, "%s:%s", knomi_config.moonraker_ip, knomi_config.moonraker_port);

            _ui_screen_change(&ui_ScreenInfo, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, NULL);
            break;
        }
        case 6:
            lv_dialog_goto_reset_wifi();
            break;
    }
}

void lv_dialog_set_custom(const char * text, String cmd);
void lv_roller_control_klipper_clicked(lv_event_t * e, uint16_t opt_id) {
    const String label[] = {
        "Restart?",
        "Firmware Restart?",
    };
    const String cmd[] = {
        "/printer/restart",
        "/printer/firmware_restart",
    };
    lv_dialog_set_custom(label[opt_id].c_str(), cmd[opt_id]);
}
uint16_t service_opt_id = 0;
void lv_roller_control_service_clicked(lv_event_t * e, uint16_t opt_id) {
    const String label[] = {
        "Start?",
        "Stop?",
        "Restart?",
    };
    String action_cmd[] = {
        "start",
        "stop",
        "restart",
    };
    if (service_opt_id >= 20) {
        return;
    }
    String cmd = "/machine/services/" + action_cmd[opt_id] + "?service=" + service_name_id[service_opt_id];
    lv_dialog_set_custom(label[opt_id].c_str(), cmd);
}

void lv_roller_control_host_clicked(lv_event_t * e, uint16_t opt_id) {
    const String label[] = {
        "Reboot?",
        "Shutdown?",
    };
    String cmd[] = {
        "/machine/reboot",
        "/machine/shutdown",
    };
    lv_dialog_set_custom(label[opt_id].c_str(), cmd[opt_id]);
}

// gesture
void lv_roller_gesture_left(lv_event_t * e) {
    // save sel opt
    cur_menu->sel_opt = lv_roller_get_selected(ui_roller);
    switch (cur_menu->this_type) {
        case UI_ROLLER_SETTING:
            _ui_screen_change(&ui_ScreenQRCode, LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, ui_ScreenQRCode_screen_init);
            break;
        case UI_ROLLER_PRINT:
            _ui_screen_change(cur_menu->previous_menu, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0, NULL);
            break;
        // case UI_ROLLER_PREHEAT:
        //     Serial.println("PREHEAT");
        //     lv_roller_preheat_clicked(e, opt_id);
        //     break;
        default:
            lv_roller_back_to_previous_menu(e);
            break;
    }
}
void lv_roller_gesture_right(lv_event_t * e) {
    // save sel opt
    cur_menu->sel_opt = lv_roller_get_selected(ui_roller);
    switch (cur_menu->this_type) {
        case UI_ROLLER_SETTING:
            _ui_screen_change(cur_menu->previous_menu, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0, NULL);
            break;
        case UI_ROLLER_PRINT:
            _ui_screen_change(cur_menu->previous_menu, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0, NULL);
            break;
        // case UI_ROLLER_PREHEAT:
        //     Serial.println("PREHEAT");
        //     lv_roller_preheat_clicked(e, opt_id);
        //     break;
        default:
            lv_roller_back_to_previous_menu(e);
            break;
    }
}

// button
void lv_roller_btn_clicked(lv_event_t * e) {
    if (cur_menu == NULL) return;

    uint16_t opt_id = lv_roller_get_selected(ui_roller);
    // save sel opt
    cur_menu->sel_opt = opt_id;
    switch (cur_menu->this_type) {
        case UI_ROLLER_SETTING:
            lv_roller_setting_clicked(e, opt_id);
            break;
        case UI_ROLLER_SETTING_THEME:
            lv_roller_theme_color_clicked(e, opt_id);
            break;
        case UI_ROLLER_CONTROL_KLIPPER:
            lv_roller_control_klipper_clicked(e, opt_id);
            break;
        case UI_ROLLER_SERVICE:
            service_opt_id = lv_roller_get_selected(ui_roller);
            lv_roller_set_type(UI_ROLLER_CONTROL_SERVICE);
            break;
        case UI_ROLLER_CONTROL_SERVICE:
            lv_roller_control_service_clicked(e, opt_id);
            break;
        case UI_ROLLER_CONTROL_HOST:
            lv_roller_control_host_clicked(e, opt_id);
            break;
        case UI_ROLLER_PREHEAT:
            lv_roller_preheat_clicked(e, opt_id);
            break;
        case UI_ROLLER_PRINT:
            lv_dialog_goto_print();
            break;
    }
}
