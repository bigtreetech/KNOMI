#include "WiFiUser.h"
#include "version.h"
#include <lvgl_gui.h>

bool dnsInit = false;

const byte DNS_PORT = 53; // 设置DNS端口号
const int webPort = 80;   // 设置Web端口号

const char *AP_SSID = "BTT-KNOMI"; // 设置AP热点名称

const char *HOST_NAME = "KNOMI"; // 设置设备名
String scanNetworksID = "";      // 用于储存扫描到的WiFi ID

IPAddress apIP(192, 168, 20, 1); // 设置AP的IP地址

String wifi_ssid = "";  // 暂时存储wifi账号密码
String wifi_pass = "";  // 暂时存储wifi账号密码
String klipper_ip = ""; // 暂时存储KlipperIP

int connectTimeOut_s = 15; // WiFi连接超时时间，单位秒

DNSServer dnsServer;       // 创建dnsServer实例
WebServer server(webPort); // 开启web服务, 创建TCP SERVER,参数:
                           // 端口号,最大连接数

// EEPROM参数存储地址位
int wifi_addr = 1; // 被写入数据的EEPROM地址编号  wifi-ssid-psw klipper

// 如开启WEB配网则可不用设置这里的参数，1为wifi ssid，2为密码，3为klipper ip
// ，4为 ap模式标志位
config_type wificonf = {{""}, {""}, {""}, {""}};

#define ROOT_HTML                                                              \
  "<!DOCTYPE html><html><head><title>WIFI</title><meta name=\"viewport\" "     \
  "content=\"width=device-width, initial-scale=1\"></head><style "             \
  "type=\"text/css\">.input{display: block; margin-top: 10px;}.input "         \
  "span{width: 100px; float: left; float: left; height: 36px; line-height: "   \
  "36px;}.input input{height: 30px;width: 200px;}.btn{width: 110px; height: "  \
  "50px; background-color: #438EF0; border-radius:8px; "                       \
  "font-size:20px;color:#ffffff;border-color: #438EF0; margin-top:16px; "      \
  "margin-left:104px;}</style><body><form method=\"POST\" "                    \
  "action=\"configwifi\"><p><span> KNOMI :-)</P><label "                       \
  "class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" "  \
  "value=\"\"></label><label class=\"input\"><span>WiFi PASS</span> <input "   \
  "type=\"text\"  name=\"pass\"><label class=\"input\"><span>Klipper "         \
  "IP</span> <input type=\"text\"  name=\"klipper\"></label><input "           \
  "class=\"btn\" type=\"submit\" name=\"submit\" value=\"SUBMIT\"> </form>"
#define ROOT_HTML_OK                                                           \
  "<!DOCTYPE html><html><head><title>WIFI SET</title><meta name=\"viewport\" " \
  "content=\"width=device-width, initial-scale=1\"></head><style "             \
  "type=\"text/css\">.c,body {text-align: center}</style><body><form "         \
  "method=\"POST\" action=\"configwifi\"></label><p><span> Submission "        \
  "successful!</P><p><span> You may now close this page.</P> </form>"
/*
 * 处理网站根目录的访问请求
 */
void handleRoot() { server.send(200, "text/html", ROOT_HTML); }

/*
 * 提交数据后的提示页面
 */
void handleConfigWifi() // 返回http状态
{
  if (server.hasArg("ssid")) // 判断是否有账号参数
  {
    LV_LOG_INFO("got ssid:");
    wifi_ssid = server.arg("ssid"); // 获取html表单输入框name名为"ssid"的内容

    strcpy(wificonf.stassid, wifi_ssid.c_str()); // 名称复制
    LV_LOG_INFO(wifi_ssid.c_str());
  } else // 没有参数
  {
    LV_LOG_INFO("error, not found ssid");
    server.send(200, "text/html",
                "<meta charset='UTF-8'>error, not found ssid"); // 返回错误页面
    return;
  }
  // 密码与账号同理
  if (server.hasArg("pass")) {
    LV_LOG_INFO("got password:");
    wifi_pass = server.arg("pass"); // 获取html表单输入框name名为"pwd"的内容

    strcpy(wificonf.stapsw, wifi_pass.c_str()); // 名称复制
    LV_LOG_INFO(wifi_pass.c_str());
  } else {
    LV_LOG_INFO("error, not found password");
    server.send(200, "text/html",
                "<meta charset='UTF-8'>error, not found password");
    return;
  }
  // klipper ip
  if (server.hasArg("klipper")) {
    LV_LOG_INFO("got KlipperIP:");
    klipper_ip =
        server.arg("klipper"); // 获取html表单输入框name名为"KlipperIP"的内容

    strcpy(wificonf.klipperip, klipper_ip.c_str()); // 名称复制

    wificonf.apmodeflag[0] = '8'; // 8 STA模式

    LV_LOG_INFO(klipper_ip.c_str());
  } else {
    LV_LOG_INFO("error, not found klipper ip");
    server.send(200, "text/html",
                "<meta charset='UTF-8'>error, not found klipper ip");
    return;
  }
  delay(200);

  // server.send(200, "text/html", "<meta charset='UTF-8'>SSID：" + wifi_ssid +
  // "<br />password:" + wifi_pass + "<br />Trying to connect Trying to connect,
  // please manually close this page."); //返回保存成功页面
  server.send(200, "text/html", ROOT_HTML_OK); // 返回保存成功页面
  delay(2000);
  WiFi.softAPdisconnect(
      true); // 参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
  server.close();          // 关闭web服务
  WiFi.softAPdisconnect(); // 在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
  LV_LOG_INFO(("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass).c_str());

  if (WiFi.status() != WL_CONNECTED) // wifi没有连接成功
  {
    LV_LOG_INFO("开始调用连接函数connectToWiFi()..");
    connectToWiFi(connectTimeOut_s);
  } else {
    LV_LOG_INFO("提交的配置信息自动连接成功..");
  }
}

/*
 * 处理404情况的函数'handleNotFound'
 */
void handleNotFound() // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
{
  handleRoot(); // 访问不存在目录则返回配置页面
}

/*
 * 进入AP模式
 */
void initSoftAP() {
  WiFi.mode(WIFI_AP); // 配置为AP模式
  WiFi.softAPConfig(apIP, apIP,
                    IPAddress(255, 255, 255, 0)); // 设置AP热点IP和子网掩码
  if (WiFi.softAP(AP_SSID)) // 开启AP热点,如需要密码则添加第二个参数
  {
    // 打印相关信息
    LV_LOG_INFO("ESP-32S SoftAP is right.");
    LV_LOG_INFO("Soft-AP IP address = ");
    LV_LOG_INFO(WiFi.softAPIP().toString().c_str()); // 接入点ip
    LV_LOG_INFO((String("MAC address = ") +
                   WiFi.softAPmacAddress().c_str()).c_str()); // 接入点mac
  } else                                             // 开启AP热点失败
  {
    LV_LOG_INFO("WiFiAP Failed");
    delay(1000);
    LV_LOG_INFO("restart now...");
    ESP.restart(); // 重启复位esp32
  }
}

/*
 * 开启DNS服务器
 */
void initDNS() {
  dnsInit = true;
  if (dnsServer.start(DNS_PORT, "*",
                      apIP)) // 判断将所有地址映射到esp32的ip上是否成功
  {
    LV_LOG_INFO("start dnsserver success.");
  } else {
    LV_LOG_INFO("start dnsserver failed.");
  }
}

void setId() {
  String short_sha = Version::getGitCommitSha1().substring(0, 8);
  String timestamp = Version::getBuildTimestamp();
  String id = short_sha + " - " + timestamp;
  ElegantOTA.setID(id.c_str());
}

/*
 * 初始化WebServer
 */
void initWebServer() {
  if (MDNS.begin("esp32")) // 给设备设定域名esp32,完整的域名是esp32.local
  {
    LV_LOG_INFO("MDNS responder started");
  }
  // 必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
  server.on(
      "/", HTTP_GET,
      handleRoot); //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
  server.on(
      "/configwifi", HTTP_POST,
      handleConfigWifi); //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理

  server.onNotFound(
      handleNotFound); // 当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
  setId();
  ElegantOTA.begin(&server);
  server.begin(); // 启动TCP SERVER

  LV_LOG_INFO("WebServer started!");
}

void initOtaServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(
      handleNotFound); // 当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
  setId();
  ElegantOTA.begin(&server);
  server.begin(); // 启动TCP SERVER
}

/*
 * 连接WiFi
 */
void connectToWiFi(int timeOut_s) {
  WiFi.hostname(HOST_NAME); // 设置设备名
  LV_LOG_INFO("进入connectToWiFi()函数");
  WiFi.mode(WIFI_STA);       // 设置为STA模式并连接WIFI
  WiFi.setAutoConnect(true); // 设置自动连接

  if (wifi_ssid != "") // wifi_ssid不为空，意味着从网页读取到wifi
  {
    LV_LOG_INFO("用web配置信息连接.");
    WiFi.begin(wifi_ssid.c_str(),
               wifi_pass.c_str()); // c_str(),获取该字符串的指针
    wifi_ssid = "";
    wifi_pass = "";
  } else // 未从网页读取到wifi
  {
    readwificonfig();
    LV_LOG_INFO("用EEROM保存的信息连接.");
    WiFi.begin(
        wificonf.stassid,
        wificonf.stapsw); // begin()不传入参数，默认连接上一次连接成功的wifi
  }

  int Connect_time = 0; // 用于连接计时，如果长时间连接不成功，复位设备
  while (WiFi.status() != WL_CONNECTED) // 等待WIFI连接成功
  {
    LV_LOG_INFO("."); // 一共打印30个点点
    // digitalWrite(LED, !digitalRead(LED));
    delay(500);
    Connect_time++;

    if (Connect_time > 2 * timeOut_s) // 长时间连接不上，重新进入配网页面
    {
      // digitalWrite(LED, LOW);
      LV_LOG_INFO(""); // 主要目的是为了换行符
      LV_LOG_INFO("WIFI autoconnect fail, start AP for webconfig now...");
      wifiConfig(); // 开始配网功能

      wifi_connect_fail = 1;
      return; // 跳出 防止无限初始化
    }
  }

  if (WiFi.status() == WL_CONNECTED) // 如果连接成功
  {
    savewificonfig();

    LV_LOG_INFO("WIFI connect Success");
    LV_LOG_INFO("SSID:%s", WiFi.SSID().c_str());
    LV_LOG_INFO(", PSW:%s\r\n", WiFi.psk().c_str());
    LV_LOG_INFO("LocalIP:");
    LV_LOG_INFO(WiFi.localIP().toString().c_str());
    LV_LOG_INFO(" ,GateIP:");
    LV_LOG_INFO(WiFi.gatewayIP().toString().c_str());

    LV_LOG_INFO("KlipperIP:");
    String str(wificonf.klipperip);
    klipper_ip = wificonf.klipperip;
    LV_LOG_INFO(klipper_ip.c_str());

    LV_LOG_INFO("WIFI status is:");
    LV_LOG_INFO(String(WiFi.status()).c_str());

    // digitalWrite(LED, HIGH);
    server.stop(); // 停止开发板所建立的网络服务器。

    wifi_connect_ok = 1; // 已连接上wifi,切换显示
  }
}

/*
 * 配置配网功能
 */
void wifiConfig() {
  initSoftAP();
  initDNS();
  initWebServer();
}

/*
 * 删除保存的wifi信息，这里的删除是删除存储在flash的信息。删除后wifi读不到上次连接的记录，需重新配网
 */
void restoreWiFi() {
  delay(500);
  esp_wifi_restore(); // 删除保存的wifi信息
  LV_LOG_INFO("连接信息已清空,准备重启设备..");
  delay(10);
}

// wifi ssid，psw保存到eeprom
void savewificonfig() {
  // 开始写入
  uint8_t *p = (uint8_t *)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++) {
    EEPROM.write(i + wifi_addr, *(p + i)); // 在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit(); // 执行写入ROM
  delay(10);
}

// 删除原有eeprom中的信息
void deletewificonfig() {
  config_type deletewifi = {
      {""}, {""}, {""}, {"5"}}; // 5 AP模式标志位 ， 8 STA模式
  uint8_t *p = (uint8_t *)(&deletewifi);
  for (int i = 0; i < sizeof(deletewifi); i++) {
    EEPROM.write(i + wifi_addr, *(p + i)); // 在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit(); // 执行写入ROM
  delay(10);
}

// 从eeprom读取WiFi信息ssid，psw
void readwificonfig() {
  uint8_t *p = (uint8_t *)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++) {
    *(p + i) = EEPROM.read(i + wifi_addr);
  }
}

/*
 * 检查wifi是否已经连接
 */
void checkConnect(bool reConnect) {
  if (WiFi.status() != WL_CONNECTED) // wifi连接失败
  {
    if (reConnect == true && WiFi.getMode() != WIFI_AP &&
        WiFi.getMode() != WIFI_AP_STA) {
      LV_LOG_INFO("WIFI未连接.");
      LV_LOG_INFO("WiFi Mode:");
      LV_LOG_INFO(String(WiFi.getMode()).c_str());
      LV_LOG_INFO("正在连接WiFi...");
      connectToWiFi(connectTimeOut_s); // 连接wifi函数
    }
  }
}

/*
 * 检测客户端DNS&HTTP请求
 */
void checkDNS_HTTP() {
  if (dnsInit) {
    dnsServer.processNextRequest(); // 检查客户端DNS请求
  }
  server.handleClient();          // 检查客户端(浏览器)http请求
}
