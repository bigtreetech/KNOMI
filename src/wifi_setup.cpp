#include <WiFi.h>
#include "DNSServer.h"
#include "knomi.h"

#define DEFAULT_KLIPPER_PORT "80"
#define DEFAULT_KLIPPER_TOOL "tool0"

#define DNS_PORT 53
DNSServer dnsServer;

// access point info
static IPAddress ap_local_ip = AP_LOCAL_IP; // access point IP
static IPAddress ap_gateway = AP_GATEWAY;   // gateway IP
static IPAddress ap_subnet = AP_SUBNET;     // subnet mask

knomi_wifi_scan_t wifi_scan;
knomi_config_t knomi_config;

void webserver_setup(void);

static uint16_t knomi_config_require = WEB_POST_NULL;

// ap info + sta info + wifi mode + wifi mode
void knomi_config_require_change(uint16_t require) {
    knomi_config_require |= require;
}

wifi_mode_t wifi_get_mode_from_string(String mode) {
    if (mode == "ap") return WIFI_MODE_AP;
    if (mode == "sta") return WIFI_MODE_STA;
    if (mode == "apsta") return WIFI_MODE_APSTA;
    return WIFI_MODE_NULL;
}

void wifi_refresh_connected(void) {
    for (uint8_t i = 0; i < wifi_scan.count; i++) {
        if (WiFi.status() == WL_CONNECTED && (strcmp(wifi_scan.ssid[i], WiFi.SSID().c_str()) == 0)) {
            wifi_scan.connected[i] = 1;
        } else {
            wifi_scan.connected[i] = 0;
        }
    }
}

wifi_auth_mode_t wifi_get_ahth_mode_from_scanned_list(void) {
    wifi_auth_mode_t mode = WIFI_AUTH_WPA2_PSK; // default valuel;
    for (uint8_t i = 0; i < wifi_scan.count; i++) {
        if (strcmp(knomi_config.sta_ssid, wifi_scan.ssid[i]) == 0) {
            mode = wifi_scan.authmode[i];
            break;
        }
    }
    return mode;
}

#include <EEPROM.h>

#define EEPROM_SIGN 0x20231212
#define EEPROM_SIGN_SIZE 4
// Init the EEPROM and restore all NV vars.
void eeprom_init(void) {
    if (!EEPROM.begin(1024)) {
        Serial.println("failed to initialise EEPROM");
        return ;
    }
    // Get sign flag
    uint32_t eeprom_sign;
    EEPROM.get<uint32_t>(0x00, eeprom_sign);
    if (eeprom_sign == EEPROM_SIGN) {
        EEPROM.get<knomi_config_t>(0x00 + EEPROM_SIGN_SIZE, knomi_config);
        Serial.println("knomi_config from EEPROM");
        Serial.print("sta_ssid: ");
        Serial.println(knomi_config.sta_ssid);
        Serial.print("sta_pwd: ");
        Serial.println(knomi_config.sta_pwd);
        Serial.print("sta_auth: ");
        Serial.println(knomi_config.sta_auth);
        Serial.print("ap_ssid: ");
        Serial.println(knomi_config.ap_ssid);
        Serial.print("ap_pwd: ");
        Serial.println(knomi_config.ap_pwd);
        Serial.print("hostname: ");
        Serial.println(knomi_config.hostname);
        Serial.print("moonraker_ip: ");
        Serial.println(knomi_config.moonraker_ip);
        Serial.print("moonraker_port: ");
        Serial.println(knomi_config.moonraker_port);
        Serial.print("moonraker_tool: ");
        Serial.println(knomi_config.moonraker_tool);
        Serial.print("mode: ");
        Serial.println(knomi_config.mode);
    } else {
        // init struct
        knomi_config.sta_ssid[0] = 0,
        knomi_config.sta_pwd[0] = 0,
        knomi_config.sta_auth = WIFI_AUTH_WPA2_PSK,
        strlcpy(knomi_config.ap_ssid, AP_SSID, sizeof(knomi_config.ap_ssid));
        strlcpy(knomi_config.ap_pwd, AP_PWD, sizeof(knomi_config.ap_pwd));
        strlcpy(knomi_config.hostname, HOSTNAME, sizeof(knomi_config.hostname));
        strlcpy(knomi_config.moonraker_ip, "", sizeof(knomi_config.moonraker_ip));
        strlcpy(knomi_config.moonraker_port, DEFAULT_KLIPPER_PORT, sizeof(knomi_config.moonraker_port));
        strlcpy(knomi_config.moonraker_tool, DEFAULT_KLIPPER_TOOL, sizeof(knomi_config.moonraker_tool));
        strlcpy(knomi_config.mode, "ap", sizeof(knomi_config.mode));
        knomi_config.theme_color = lv_color_hex(LV_DEFAULT_COLOR);

        EEPROM.put<uint32_t>(0x00, EEPROM_SIGN);
        EEPROM.put<knomi_config_t>(0x00 + EEPROM_SIGN_SIZE, knomi_config);
        EEPROM.commit();
        Serial.println("knomi_config from default & INIT EEPROM");
    }
}

void knomi_factory_reset(void) {
    EEPROM.put<uint32_t>(0x00, EEPROM_SIGN + 1); // Any value other than 'EEPROM_SIGN'
    EEPROM.commit();
    Serial.println("EEPROM reset factory");
    ESP.restart();
}


static wifi_status_t wifi_status = WIFI_STATUS_INIT;

wifi_status_t wifi_get_connect_status(void) {
    return wifi_status;
}

static p_function_t wifi_scan_refresh_callback = NULL;

void wifi_scan_refresh_set_callback(p_function_t cb) {
    wifi_scan_refresh_callback = cb;
}

void wifi_scan_refresh(void) {
    int16_t n = WiFi.scanComplete();
    if (n >= 0) {
        Serial.println("Scan ok!");
        wifi_scan.count = min(n, (int16_t)SCAN_SSIDS_NUM);
        for (int i = 0; i < wifi_scan.count; i++) {
            strlcpy(wifi_scan.ssid[i], WiFi.SSID(i).c_str(), sizeof(wifi_scan.ssid[i]));
            wifi_scan.rssi[i] = WiFi.RSSI(i);
            wifi_scan.authmode[i] = WiFi.encryptionType(i);
            if (WiFi.status() == WL_CONNECTED && (strcmp(wifi_scan.ssid[i], WiFi.SSID().c_str()) == 0)) {
                wifi_scan.connected[i] = 1;
            } else {
                wifi_scan.connected[i] = 0;
            }
        }
        WiFi.scanDelete();
        if (wifi_scan_refresh_callback != NULL) {
            wifi_scan_refresh_callback();
            wifi_scan_refresh_callback = NULL;
        }
    } else if (n == WIFI_SCAN_FAILED) {
        // scanDeleted or failed
        // Serial.println("Scan faile!");
    } else if (n == WIFI_SCAN_RUNNING) {
        Serial.println("Scaning...");
    }
}


void eeprom_write_knomi_config(void) {
    Serial.println("knomi_config Save to EEPROM");
    Serial.print("sta_ssid: ");
    Serial.println(knomi_config.sta_ssid);
    Serial.print("sta_pwd: ");
    Serial.println(knomi_config.sta_pwd);
    Serial.print("sta_auth: ");
    Serial.println(knomi_config.sta_auth);
    Serial.print("ap_ssid: ");
    Serial.println(knomi_config.ap_ssid);
    Serial.print("ap_pwd: ");
    Serial.println(knomi_config.ap_pwd);
    Serial.print("hostname: ");
    Serial.println(knomi_config.hostname);
    Serial.print("moonraker_ip: ");
    Serial.println(knomi_config.moonraker_ip);
    Serial.print("moonraker_port: ");
    Serial.println(knomi_config.moonraker_port);
    Serial.print("moonraker_tool: ");
    Serial.println(knomi_config.moonraker_tool);
    Serial.print("mode: ");
    Serial.println(knomi_config.mode);
    EEPROM.put<knomi_config_t>(0x00 + EEPROM_SIGN_SIZE, knomi_config);
    EEPROM.commit();
}

void wifi_config_loop(bool first_setup) {
    if (knomi_config_require & EEPROM_PARA_CHANGED) {
        eeprom_write_knomi_config();
        Serial.println("knomi_config write to EEPROM");
        Serial.print("knomi_config_require: ");
        Serial.println(knomi_config_require);
        Serial.print("para: ");
        Serial.println(knomi_config_require & EEPROM_PARA_CHANGED);
    }
    if (knomi_config_require & LOCAL_POST_LV_THEME_COLOR) {
        knomi_config_require &= ~LOCAL_POST_LV_THEME_COLOR;
    }

    if (first_setup) {
        knomi_config_require = WEB_POST_LOCAL_HOSTNAME | \
                              WEB_POST_WIFI_CONFIG_AP | \
                              WEB_POST_WIFI_CONFIG_STA | \
                              WEB_POST_WIFI_CONFIG_MODE;
    }

    // TODO: mDNS
    // hostname of ESP
    if (knomi_config_require & WEB_POST_LOCAL_HOSTNAME) {
        knomi_config_require &= ~WEB_POST_LOCAL_HOSTNAME;
        // must before WiFi.begin()
        WiFi.hostname(knomi_config.hostname);
        Serial.print("mdns hostname: ");
        Serial.println(knomi_config.hostname);
    }

restart:
    // WIFI mode
    wifi_mode_t wifi_mode = wifi_get_mode_from_string(knomi_config.mode);
    if (knomi_config_require & WEB_POST_WIFI_CONFIG_MODE) {
        knomi_config_require &= ~WEB_POST_WIFI_CONFIG_MODE;
        wifi_mode_t last_mode = WiFi.getMode();
        WiFi.mode(wifi_mode);  /*ESP32 Access point configured*/
        wifi_refresh_connected();

        if (last_mode == WIFI_MODE_AP) {
            if (wifi_mode ==  WIFI_MODE_STA || wifi_mode == WIFI_MODE_APSTA) {
                knomi_config_require |= WEB_POST_WIFI_CONFIG_STA;
            }
        } else if (last_mode == WIFI_MODE_STA) {
            if (wifi_mode ==  WIFI_MODE_AP || wifi_mode == WIFI_MODE_APSTA) {
                knomi_config_require |= WEB_POST_WIFI_CONFIG_AP;
            }
        }
    }
    if (knomi_config.sta_ssid[0] == 0 || wifi_mode == WIFI_MODE_AP) {
        wifi_status = WIFI_STATUS_ERROR;
    }

    // access point setup
    if (knomi_config_require & WEB_POST_WIFI_CONFIG_AP) {
        knomi_config_require &= ~WEB_POST_WIFI_CONFIG_AP;
        if (wifi_mode == WIFI_MODE_AP || wifi_mode == WIFI_MODE_APSTA) {
            Serial.println("knomi_config_require: AP");
            Serial.print("ap ssid: ");
            Serial.println(knomi_config.ap_ssid);
            Serial.print("ap pwd: ");
            Serial.println(knomi_config.ap_pwd);
            WiFi.softAPConfig(ap_local_ip, ap_gateway, ap_subnet);
            if (WiFi.softAP(knomi_config.ap_ssid, knomi_config.ap_pwd)) {
                Serial.print("ap ip: ");
                Serial.println(WiFi.softAPIP());
                Serial.print("ap mac: ");
                Serial.println(WiFi.softAPmacAddress().c_str());
                Serial.println();
            } else {
                Serial.println("access point create failed!!!\r\n");
            }
            // dns for captive portal
            bool ret = dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
            Serial.print("dnsServer: ");
            Serial.println(ret);
        }
    }

    // station connect
    if (knomi_config_require & WEB_POST_WIFI_CONFIG_STA) {
        knomi_config_require &= ~WEB_POST_WIFI_CONFIG_STA;
        Serial.println("knomi_config_require: STA");
        if (wifi_mode == WIFI_MODE_STA || wifi_mode == WIFI_MODE_APSTA) {
            Serial.println();
            Serial.println();
            Serial.print("sta ssid: ");
            Serial.println(knomi_config.sta_ssid);
            Serial.print("sta pwd: ");
            Serial.println(knomi_config.sta_pwd);
            WiFi.setMinSecurity(knomi_config.sta_auth);
            wl_status_t n = WiFi.begin(knomi_config.sta_ssid, knomi_config.sta_pwd);  /*Connecting to Defined Access point*/
            wifi_status = WIFI_STATUS_CONNECTING;
            uint32_t timeout = millis() + WIFI_STA_TIMEOUT;
            while (millis() < timeout) {
                if (WiFi.status() == WL_CONNECTED) {
                    wifi_status = WIFI_STATUS_CONNECTED;
                    break;
                }
                Serial.print(".");
                delay(100);
            }
            if (wifi_status != WIFI_STATUS_CONNECTED) {
                Serial.println("sta connect failed!!!");
                wifi_status = WIFI_STATUS_ERROR;
                // reset wifi mode to "ap"
                strlcpy(knomi_config.mode, "ap", sizeof(knomi_config.mode));
                knomi_config_require |= WEB_POST_WIFI_CONFIG_MODE;
                goto restart;
            }
            wifi_refresh_connected();
            Serial.print("sta ip: ");
            Serial.println(WiFi.localIP());   /*Printing IP address of Connected network*/
        }
    }

    //
    if (knomi_config_require & WEB_POST_MOONRAKER) {
        knomi_config_require &= ~WEB_POST_MOONRAKER;
        Serial.print("klipper ip: ");
        Serial.println(knomi_config.moonraker_ip);
        Serial.print("port: ");
        Serial.println(knomi_config.moonraker_port);
        Serial.print("tool: ");
        Serial.println(knomi_config.moonraker_tool);
    }

    // refresh wifi scan
    if (knomi_config_require & WEB_POST_WIFI_REFRESH) {
        knomi_config_require &= ~WEB_POST_WIFI_REFRESH;
        WiFi.scanNetworks(true, false, true, 75U);
    }

    // restart
    if (knomi_config_require & WEB_POST_RESTART) {
        ESP.restart();
    }
}

void wifi_task(void * parameter) {

    eeprom_init();
    wifi_config_loop(true);
    WiFi.scanNetworks(true, false, true, 75U);
    webserver_setup();

    while (1) {
        wifi_scan_refresh();
        wifi_config_loop(false);

        wl_status_t s = WiFi.status();
        switch (s) {
            case WL_CONNECTED:
                wifi_status = WIFI_STATUS_CONNECTED;
                break;
            case WL_NO_SSID_AVAIL:
            case WL_DISCONNECTED:
                if (wifi_status == WIFI_STATUS_CONNECTED)
                    wifi_status = WIFI_STATUS_DISCONNECT;
                break;
        }

        wifi_mode_t m = WiFi.getMode();
        if (m == WIFI_MODE_AP || m == WIFI_MODE_APSTA) {
            dnsServer.processNextRequest();
        }

        delay(100);
    }
}
