#include "WiFiUser.h"
#include <lvgl_gui.h>

bool dnsInit = false;

WifiConfig* wificonfig = new WifiConfig();

const byte DNS_PORT = 53; // 设置DNS端口号

const char *AP_SSID = "BTT-KNOMI"; // 设置AP热点名称

const char *HOST_NAME = "KNOMI"; // 设置设备名

IPAddress apIP(192, 168, 20, 1); // 设置AP的IP地址

DNSServer dnsServer;       // 创建dnsServer实例

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

/*
 * 连接WiFi
 */
void connectToWiFi() {
  const int timeOut_s = 15; // WiFi连接超时时间，单位秒

  WiFi.hostname(HOST_NAME); // 设置设备名
  LV_LOG_INFO("进入connectToWiFi()函数");
  WiFi.mode(WIFI_STA);       // 设置为STA模式并连接WIFI
  WiFi.setAutoConnect(true); // 设置自动连接

  if (wificonfig->getSSID() != "") // wifi_ssid不为空，意味着从网页读取到wifi
  {
    LV_LOG_INFO("用web配置信息连接.");
    WiFi.begin(wificonfig->getSSID(), wificonfig->getPassword()); // c_str(),获取该字符串的指针
    wificonfig->setSSID("");
    wificonfig->setPassword("");
  } else // 未从网页读取到wifi
  {
    wificonfig->ReadConfig();
    LV_LOG_INFO("用EEROM保存的信息连接.");
    WiFi.begin(wificonfig->getSSID(), wificonfig->getPassword()); // begin()不传入参数，默认连接上一次连接成功的wifi
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
    wificonfig->SaveConfig();

    LV_LOG_INFO("WIFI connect Success");
    LV_LOG_INFO("SSID:%s", WiFi.SSID().c_str());
    LV_LOG_INFO(", PSW:%s\r\n", WiFi.psk().c_str());
    LV_LOG_INFO("LocalIP:");
    LV_LOG_INFO(WiFi.localIP().toString().c_str());
    LV_LOG_INFO(" ,GateIP:");
    LV_LOG_INFO(WiFi.gatewayIP().toString().c_str());

    LV_LOG_INFO("KlipperIP:");
    LV_LOG_INFO(wificonfig->getKlipperIp().c_str());

    LV_LOG_INFO("WIFI status is:");
    LV_LOG_INFO(String(WiFi.status()).c_str());

    wifi_connect_ok = 1; // 已连接上wifi,切换显示
  }
}

/*
 * 配置配网功能
 */
void wifiConfig() {
  initSoftAP();
  initDNS();
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
      connectToWiFi(); // 连接wifi函数
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
}
