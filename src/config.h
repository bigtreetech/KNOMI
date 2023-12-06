#ifndef CONFIG_H
#define CONFIG_H

#include "pinout.h"

 // default 80 for http
#define SERVER_PORT 80

#define SCAN_SSIDS_NUM 10     //  The max number of SSIDs that we will scan for.

#define HOSTNAME "KNOMI"
#define AP_SSID "BTT-KNOMI" // Create a SSID for BTT KNOMI Access Point
#define AP_PWD "" // Default no password
#define AP_LOCAL_IP IPAddress(192, 168, 20, 1) // access point IP
#define AP_GATEWAY  IPAddress(192, 168, 20, 1) // gateway IP
#define AP_SUBNET   IPAddress(255, 255, 255, 0) // subnet mask

// #define STA_TEST_SSID "biqu-m"
// #define STA_TEST_PWD  "biqu2020"

typedef void (*p_function_t)(void);

#include <WiFi.h>
#include "lvgl.h"
typedef struct {
    char sta_ssid[33];
    char sta_pwd[65];
    wifi_auth_mode_t sta_auth;
    char ap_ssid[33];
    char ap_pwd[65];
    char hostname[16];
    char moonraker_ip[16]; // "192.168.255.255" max len = 16
    char mode[6]; // "ap":WIFI_MODE_AP, "sta":WIFI_MODE_STA, "apsta":WIFI_MODE_APSTA
    lv_color_t theme_color;
} knomi_config_t;

typedef struct {
    int32_t rssi[SCAN_SSIDS_NUM]; // RSSI returned RSSI values
    char ssid[SCAN_SSIDS_NUM][65]; // SSIDs returned by the wifi scan.
    wifi_auth_mode_t authmode[SCAN_SSIDS_NUM]; // encryptionType
    uint8_t connected[SCAN_SSIDS_NUM];
    uint8_t count; // Number of WiFi Scanned
} knomi_wifi_scan_t;

extern knomi_config_t knomi_config;
extern knomi_wifi_scan_t wifi_scan;


#define WEB_POST_NULL                 (0)
#define WEB_POST_LOCAL_HOSTNAME       BIT(0)
#define WEB_POST_WIFI_CONFIG_MODE     BIT(1)
#define WEB_POST_WIFI_CONFIG_AP       BIT(2)
#define WEB_POST_WIFI_CONFIG_STA      BIT(3)
#define WEB_POST_MOONRAKER_IP         BIT(4)
#define WEB_POST_WIFI_REFRESH         BIT(5)
#define WEB_POST_RESTART              BIT(6)

#define LOCAL_POST_LV_THEME_COLOR     BIT(7)
#define EEPROM_PARA_CHANGED     (WEB_POST_WIFI_CONFIG_AP   | \
                                 WEB_POST_WIFI_CONFIG_STA  | \
                                 WEB_POST_WIFI_CONFIG_MODE | \
                                 WEB_POST_LOCAL_HOSTNAME   | \
                                 WEB_POST_MOONRAKER_IP     | \
                                 LOCAL_POST_LV_THEME_COLOR)


void knomi_config_require_change(uint16_t require);
void wifi_scan_refresh_set_callback(p_function_t cb);
wifi_auth_mode_t wifi_get_ahth_mode_from_scanned_list(void);

void knomi_factory_reset(void);

typedef enum {
    WIFI_STATUS_ERROR, // no station ssid or connect failed
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_DISCONNECT,
    WIFI_STATUS_INIT,
}wifi_status_t;

wifi_status_t wifi_get_connect_status(void);

#define WIFI_STA_TIMEOUT 15000  // 15s

extern int32_t lis2dw12_acc[3];

// BTT red color for UI (RGB888)
#define LV_32BIT_BTT_RED    0xC02F30
#define LV_32BIT_BTT_BLUE   0x209ADE
#define LV_32BIT_BTT_PURPLE 0xA91DDA
#define LV_32BIT_BTT_GREEN  0x5DA910
#define LV_DEFAULT_COLOR    LV_32BIT_BTT_RED

// Macros for array numbers
#define ACOUNT(n) (sizeof(n) / sizeof(n[0]))
// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

// Preheat
#define PREHEAT_0_NOZZLE 0
#define PREHEAT_0_BED    0
#define PREHEAT_0_LABEL "CoolDown"
#define PREHEAT_1_NOZZLE 200
#define PREHEAT_1_BED    60
#define PREHEAT_1_LABEL "PLA:" STRINGIFY(PREHEAT_1_NOZZLE) "/" STRINGIFY(PREHEAT_1_BED) "℃"
#define PREHEAT_2_NOZZLE 240
#define PREHEAT_2_BED    70
#define PREHEAT_2_LABEL "PETG:" STRINGIFY(PREHEAT_2_NOZZLE) "/" STRINGIFY(PREHEAT_2_BED) "℃"
#define PREHEAT_3_NOZZLE 230
#define PREHEAT_3_BED    90
#define PREHEAT_3_LABEL "ABS:" STRINGIFY(PREHEAT_3_NOZZLE) "/" STRINGIFY(PREHEAT_3_BED) "℃"
#define PREHEAT_4_NOZZLE 170
#define PREHEAT_4_BED    50
#define PREHEAT_4_LABEL "WOOD:" STRINGIFY(PREHEAT_4_NOZZLE) "/" STRINGIFY(PREHEAT_4_BED) "℃"
#define PREHEAT_5_NOZZLE 220
#define PREHEAT_5_BED    50
#define PREHEAT_5_LABEL "TPU:" STRINGIFY(PREHEAT_5_NOZZLE) "/" STRINGIFY(PREHEAT_5_BED) "℃"
#define PREHEAT_6_NOZZLE 250
#define PREHEAT_6_BED    90
#define PREHEAT_6_LABEL "NYLON:" STRINGIFY(PREHEAT_6_NOZZLE) "/" STRINGIFY(PREHEAT_6_BED) "℃"

// Extrude length, mm
#define EXTRUDE_MM_0 1
#define EXTRUDE_MM_0_LABEL STRINGIFY(EXTRUDE_MM_0)"mm"
#define EXTRUDE_MM_1 5
#define EXTRUDE_MM_1_LABEL STRINGIFY(EXTRUDE_MM_1)"mm"
#define EXTRUDE_MM_2 10
#define EXTRUDE_MM_2_LABEL STRINGIFY(EXTRUDE_MM_2)"mm"
#define EXTRUDE_MM_3 50
#define EXTRUDE_MM_3_LABEL STRINGIFY(EXTRUDE_MM_3)"mm"
#define EXTRUDE_MM_4 100
#define EXTRUDE_MM_4_LABEL STRINGIFY(EXTRUDE_MM_4)"mm"
// mm/s to mm/min
#define MMS_TO_MMM(n) ((n) * 60)
// Extrude speed, mm/s
#define EXTRUDE_MM_S_0 1
#define EXTRUDE_MM_S_0_LABEL STRINGIFY(EXTRUDE_MM_S_0)"mm/s"
#define EXTRUDE_MM_S_1 5
#define EXTRUDE_MM_S_1_LABEL STRINGIFY(EXTRUDE_MM_S_1)"mm/s"
#define EXTRUDE_MM_S_2 10
#define EXTRUDE_MM_S_2_LABEL STRINGIFY(EXTRUDE_MM_S_2)"mm/s"
#define EXTRUDE_MM_S_3 50
#define EXTRUDE_MM_S_3_LABEL STRINGIFY(EXTRUDE_MM_S_3)"mm/s"
#define EXTRUDE_MM_S_4 100
#define EXTRUDE_MM_S_4_LABEL STRINGIFY(EXTRUDE_MM_S_4)"mm/s"

#endif
