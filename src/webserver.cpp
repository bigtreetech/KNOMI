#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <ESPmDNS.h>

#include "knomi.h"

static AsyncWebServer server(SERVER_PORT);

const char captive_html[] PROGMEM = R"rawliteral(<html>
<head>
<meta http-equiv="refresh" content="2;url=/" />
<title>For makers! By makers!</title>
</head>
<body>
You've successfully connected to the BTT KNOMI Screen. Click <a href="/">here</a> to go to the homepage.
</body>
</html>)rawliteral";

// This is a wrapper for a normal Async handler that allows the captive portal to intercept
// all requests regardless of their destination.
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", captive_html);
  }
};

// This function will populate variables on the main html index page.
// Variables include WiFi SSIDs/RSSIs and the current mode.
String knomi_html_processor(const String& var){
    String value = "";
    if (var == "wifi_list") {
        for (uint8_t i = 0; i < wifi_scan.count; i++) {
            value += R"rawliteral(<tr class="showpop" data-modal="modalOne"> <td class="ssid" >)rawliteral";
            value += wifi_scan.ssid[i];
            value += "</td> <td>";
            value += wifi_scan.rssi[i];
            value += "</td> <td>";
            value += wifi_scan.connected[i] ? "connected" : "";
            value += "</td> </tr>";
        }
    } else if (var == "ip") {
        String moonraker_ip = knomi_config.moonraker_ip;
        value = (moonraker_ip.isEmpty() ? "" : "value=") + moonraker_ip;
    } else if (var == "port") {
        String port = knomi_config.moonraker_port;
        value = port.isEmpty() ? "" : "value=" + port;
    } else if (var == "tool") {
        String tool = knomi_config.moonraker_tool;
        value = tool.isEmpty() ? "" : "value=" + tool;
    } else if (var == "ap_ssid") {
        String ap_ssid = knomi_config.ap_ssid;
        value = (ap_ssid.isEmpty() ? "" : "value=") + ap_ssid;
    } else if (var == "ap_password") {
        String ap_pwd = knomi_config.ap_pwd;
        value = (ap_pwd.isEmpty() ? "" : "value=") + ap_pwd;
    } else if (var == "hostname") {
        String hostname = knomi_config.hostname;
        value = hostname.isEmpty() ? "" : "value=" + hostname;
    } else  if (var == knomi_config.mode) {
        value = "selected";
    }
    return value;    // Could just be something between two normal $ signs in the HTML...
}

/*
 * mDNS
 * OTA
 */
#include "favicon.h"
#include "index_html.h"
typedef struct {
    const char * name;
    char * value;
    uint8_t v_len;
    uint8_t require;
} web_post_info_t;

web_post_info_t web_post_info[] = {
    {
        .name = "ssid",
        .value = knomi_config.sta_ssid,
        .v_len = sizeof(knomi_config.sta_ssid),
        .require = WEB_POST_WIFI_CONFIG_STA,
    },
    {
        .name = "password",
        .value = knomi_config.sta_pwd,
        .v_len = sizeof(knomi_config.sta_pwd),
        .require = WEB_POST_WIFI_CONFIG_STA,
    },
    {
        .name = "mode",
        .value = knomi_config.mode,
        .v_len = sizeof(knomi_config.mode),
        .require = WEB_POST_WIFI_CONFIG_MODE,
    },
    {
        .name = "ap_ssid",
        .value = knomi_config.ap_ssid,
        .v_len = sizeof(knomi_config.ap_ssid),
        .require = WEB_POST_WIFI_CONFIG_AP,
    },
    {
        .name = "ap_password",
        .value = knomi_config.ap_pwd,
        .v_len = sizeof(knomi_config.ap_pwd),
        .require = WEB_POST_WIFI_CONFIG_AP,
    },
    {
        .name = "hostname",
        .value = knomi_config.hostname,
        .v_len = sizeof(knomi_config.hostname),
        .require = WEB_POST_LOCAL_HOSTNAME,
    },
    {
        .name = "ip",
        .value = knomi_config.moonraker_ip,
        .v_len = sizeof(knomi_config.moonraker_ip),
        .require = WEB_POST_MOONRAKER,
    },
    {
        .name = "port",
        .value = knomi_config.moonraker_port,
        .v_len = sizeof(knomi_config.moonraker_port),
        .require = WEB_POST_MOONRAKER,
    },
    {
        .name = "tool",
        .value = knomi_config.moonraker_tool,
        .v_len = sizeof(knomi_config.moonraker_tool),
        .require = WEB_POST_MOONRAKER,
    },
    {
        .name = "refresh",
        .value = NULL,
        .v_len = 0,
        .require = WEB_POST_WIFI_REFRESH,
    },
    {
        .name = "restart",
        .value = NULL,
        .v_len = 0,
        .require = WEB_POST_RESTART,
    },
};

static AsyncWebServerRequest * wifi_refresh_request = NULL;
void webserver_wifi_refresh_callback(void) {
    if (wifi_refresh_request == NULL) return;
    wifi_refresh_request->send_P(200, "text/html", index_html, knomi_html_processor);
    wifi_refresh_request = NULL;
}

void webserver_setup(void) {

    if(!MDNS.begin(knomi_config.hostname)) {
        Serial.println("Error starting mDNS");
    } else {
        Serial.println("mDNS start ok!");
    }
    AsyncElegantOTA.begin(&server);

    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse_P(200, "image/x-icon", btt_logo_only_ico, sizeof(btt_logo_only_ico));
        request->send(response);
    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, knomi_html_processor);
    });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
        uint8_t post_require = WEB_POST_NULL;
        int paramsNr = request->params();
        for (int i = 0; i < paramsNr; i++) {
            AsyncWebParameter* p = request->getParam(i);
            Serial.printf("name: %s\r\n", p->name().c_str());
            Serial.printf("value: %s\r\n", p->value().c_str());

            for (uint8_t i = 0; i < ACOUNT(web_post_info); i++) {
                if (strcmp(web_post_info[i].name, p->name().c_str()) == 0) {
                    if (web_post_info[i].value == NULL) {
                        post_require |= web_post_info[i].require;
                        break;
                    }
                    if (strcmp(web_post_info[i].value, p->value().c_str()) != 0) {
                        post_require |= web_post_info[i].require;
                        strlcpy(web_post_info[i].value, p->value().c_str(), web_post_info[i].v_len);
                    }
                    break;
                }
            }
        }
        if ((post_require & WEB_POST_WIFI_CONFIG_STA)) {
            if (strcmp(knomi_config.mode, "ap") == 0) {
                strlcpy(knomi_config.mode, "sta", sizeof(knomi_config.mode));
                post_require |= WEB_POST_WIFI_CONFIG_MODE;
            }
            knomi_config.sta_auth = wifi_get_ahth_mode_from_scanned_list();
        }

        knomi_config_require_change(post_require);

        if (post_require & WEB_POST_WIFI_CONFIG_STA) {
            String sta_ssid = knomi_config.sta_ssid;
            String sta_pwd = knomi_config.sta_pwd;
            request->send(200, "text/html", "SSID: " + sta_ssid + "<br>PWD: " + sta_pwd + \
                    "<br>The BTT KNOMI will now attempt to connect to the specified network.<br>If it fails after 15s then this access point will be re-launched and you can connect to it to try again. <br><a href=\"/\">Return to Home Page</a>");
        } else if (post_require & WEB_POST_LOCAL_HOSTNAME){
            request->send(200, "text/html", "The hostname needs to be restarted before it takes effect.<br>Please return to the home page and manually restart. <br><a href=\"/\">Return to Home Page</a>");
        } else if (post_require & WEB_POST_RESTART){
            request->send(200, "text/html", "KNOMI is restarting, please wait for the restart to complete and re-establish the connection. <br><a href=\"/\">Return to Home Page</a>");
        } else if (post_require & WEB_POST_WIFI_REFRESH) {
            wifi_refresh_request = request;
            wifi_scan_refresh_set_callback(webserver_wifi_refresh_callback);
        } else {
            request->send_P(200, "text/html", index_html, knomi_html_processor);
        }
    });

    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server.begin();
}
