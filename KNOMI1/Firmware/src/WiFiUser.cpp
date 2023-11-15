#include "WiFiUser.h"
#include <lvgl_gui.h>
#include <test.h>

const byte DNS_PORT = 53;                  //设置DNS端口号
const int webPort = 80;                    //设置Web端口号
 
const char* AP_SSID  = "BTT-KNOMI";        //设置AP热点名称
//const char* AP_PASS  = "";               //这里不设置设置AP热点密码
 
const char* HOST_NAME = "KNOMI";        //设置设备名
String scanNetworksID = "";                //用于储存扫描到的WiFi ID

String scanNetworksID1 = "no networks found";                
String scanNetworksID2 = "";
String scanNetworksID3 = "";                 

IPAddress apIP(192, 168, 20, 1);            //设置AP的IP地址
 
String wifi_ssid = "";                     //暂时存储wifi账号密码
String wifi_pass = "";                     //暂时存储wifi账号密码
String klipper_ip = "";                     //暂时存储KlipperIP

int connectTimeOut_s = 15;   //WiFi连接超时时间，单位秒
const int LED = 2;                         //设置LED引脚
 
DNSServer dnsServer;                       //创建dnsServer实例
WebServer server(webPort);                 //开启web服务, 创建TCP SERVER,参数: 端口号,最大连接数


//EEPROM参数存储地址位
int wifi_addr = 1; //被写入数据的EEPROM地址编号  wifi-ssid-psw klipper



//如开启WEB配网则可不用设置这里的参数，1为wifi ssid，2为密码，3为klipper ip ，4为 ap模式标志位
config_type wificonf ={{""},{""},{""},{""}};


// #define ROOT_HTML  "<!DOCTYPE html><html><head><title>WIFI</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 110px; height: 50px; background-color: #438EF0; border-radius:8px; font-size:20px;color:#ffffff;border-color: #438EF0; margin-top:16px; margin-left:104px;}</style><body><form method=\"POST\" action=\"configwifi\"><p><span> BIQU TFT</P><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi PASS</span> <input type=\"text\"  name=\"pass\"><label class=\"input\"><span>Klipper IP</span> <input type=\"text\"  name=\"klipper\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> </form>"
// #define ROOT_HTML_OK  "<!DOCTYPE html><html><head><title>WIFI SET</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.c,body {text-align: center}</style><body><form method=\"POST\" action=\"configwifi\"></label><p><span> submit successfully!</P><p><span> please manually close this page.</P> </form>"
#define ROOT_HTML  "<!DOCTYPE html><html><head><title>WIFI</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 110px; height: 50px; background-color: #438EF0; border-radius:8px; font-size:20px;color:#ffffff;border-color: #438EF0; margin-top:16px; margin-left:104px;}</style><body><form method=\"POST\" action=\"configwifi\"><p><span> KNOMI :-)</P><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi PASS</span> <input type=\"text\"  name=\"pass\"><label class=\"input\"><span>Klipper IP</span> <input type=\"text\"  name=\"klipper\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"SUBMIT\"> </form>"
#define ROOT_HTML_OK  "<!DOCTYPE html><html><head><title>WIFI SET</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.c,body {text-align: center}</style><body><form method=\"POST\" action=\"configwifi\"></label><p><span> Submission successful!</P><p><span> You may now close this page.</P> </form>"
/*
 * 处理网站根目录的访问请求
 */
void handleRoot() 
{
  // if (server.hasArg("selectSSID")) {
  //   server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");   //scanNetWprksID是扫描到的wifi
  // } else {
  //   server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");   
  // }
  if (server.hasArg("selectSSID")) {
    server.send(200, "text/html", ROOT_HTML);   
  } else {
    server.send(200, "text/html", ROOT_HTML);   
  }
}
 
/*
 * 提交数据后的提示页面
 */
void handleConfigWifi()               //返回http状态
{
  if (server.hasArg("ssid"))          //判断是否有账号参数
  {
    Serial.print("got ssid:");
    wifi_ssid = server.arg("ssid");   //获取html表单输入框name名为"ssid"的内容

    strcpy(wificonf.stassid,wifi_ssid.c_str());//名称复制
    Serial.println(wifi_ssid);
  } 
  else                                //没有参数
  { 
    Serial.println("error, not found ssid");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); //返回错误页面
    return;
  }
  //密码与账号同理
  if (server.hasArg("pass")) 
  {
    Serial.print("got password:");
    wifi_pass = server.arg("pass");  //获取html表单输入框name名为"pwd"的内容
    
    strcpy(wificonf.stapsw,wifi_pass.c_str());//名称复制
    Serial.println(wifi_pass);
  } 
  else 
  {
    Serial.println("error, not found password");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  //klipper ip
  if (server.hasArg("klipper")) 
  {
    Serial.print("got KlipperIP:");
    klipper_ip = server.arg("klipper");  //获取html表单输入框name名为"KlipperIP"的内容

    strcpy(wificonf.klipperip,klipper_ip.c_str());//名称复制  

    wificonf.apmodeflag[0] = '8';//8 STA模式

    Serial.println(klipper_ip);
  } 
  else 
  {
    Serial.println("error, not found klipper ip");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found klipper ip");
    return;
  }
  delay(200);

  // server.send(200, "text/html", "<meta charset='UTF-8'>SSID：" + wifi_ssid + "<br />password:" + wifi_pass + "<br />Trying to connect Trying to connect, please manually close this page."); //返回保存成功页面
  server.send(200, "text/html",ROOT_HTML_OK); //返回保存成功页面
  delay(2000);
  WiFi.softAPdisconnect(true);     //参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
  server.close();                  //关闭web服务
  WiFi.softAPdisconnect();         //在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
  Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);
 
  if (WiFi.status() != WL_CONNECTED)    //wifi没有连接成功
  {
    Serial.println("开始调用连接函数connectToWiFi()..");
    connectToWiFi(connectTimeOut_s);
  } 
  else {
    Serial.println("提交的配置信息自动连接成功..");
  }
}
 
/*
 * 处理404情况的函数'handleNotFound'
 */
void handleNotFound()           // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
{           
  handleRoot();                 //访问不存在目录则返回配置页面
  //   server.send(404, "text/plain", "404: Not found");
}
 
/*
 * 进入AP模式
 */
void initSoftAP() {
  WiFi.mode(WIFI_AP);                                           //配置为AP模式
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   //设置AP热点IP和子网掩码
  if (WiFi.softAP(AP_SSID))                                     //开启AP热点,如需要密码则添加第二个参数
  {                           
    //打印相关信息
    Serial.println("ESP-32S SoftAP is right.");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());                                                //接入点ip
    Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());    //接入点mac
  } 
  else                                                  //开启AP热点失败
  { 
    Serial.println("WiFiAP Failed");
    delay(1000);
    Serial.println("restart now...");
    ESP.restart();                                      //重启复位esp32
  }
}
 
/*
 * 开启DNS服务器
 */
void initDNS() 
{
  if (dnsServer.start(DNS_PORT, "*", apIP))   //判断将所有地址映射到esp32的ip上是否成功
  {
    Serial.println("start dnsserver success.");
  } else {
    Serial.println("start dnsserver failed.");
  }
}
 
/*
 * 初始化WebServer
 */
void initWebServer() 
{
  if (MDNS.begin("esp32"))      //给设备设定域名esp32,完整的域名是esp32.local
  {
    Serial.println("MDNS responder started");
  }
  //必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);                      //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
  server.on("/configwifi", HTTP_POST, handleConfigWifi);     //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
                                                            
  server.onNotFound(handleNotFound);                         //当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
 
  server.begin();                                           //启动TCP SERVER
 
  Serial.println("WebServer started!");
}
 
/*
 * 扫描附近的WiFi，为了显示在配网界面
 */
bool scanWiFi() {
  Serial.println("scan start");
  Serial.println("--------->");
  // 扫描附近WiFi
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    scanNetworksID = "no networks found";
    return false;
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
      
      if(i==0)scanNetworksID1 =WiFi.SSID(i);
      if(i==1)scanNetworksID2 =WiFi.SSID(i);
      if(i==2)scanNetworksID3 =WiFi.SSID(i);

      delay(10);
    }
    return true;
  }
}
 
/*
 * 连接WiFi
 */
void connectToWiFi(int timeOut_s) {
  WiFi.hostname(HOST_NAME);             //设置设备名
  Serial.println("进入connectToWiFi()函数");
  WiFi.mode(WIFI_STA);                        //设置为STA模式并连接WIFI
  WiFi.setAutoConnect(true);                  //设置自动连接    
  
  if (wifi_ssid != "")                        //wifi_ssid不为空，意味着从网页读取到wifi
  {
    Serial.println("用web配置信息连接.");
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str()); //c_str(),获取该字符串的指针
    wifi_ssid = "";
    wifi_pass = "";
  } 
  else                                        //未从网页读取到wifi
  {
    readwificonfig();
    Serial.println("用EEROM保存的信息连接.");
    WiFi.begin(wificonf.stassid,wificonf.stapsw); //begin()不传入参数，默认连接上一次连接成功的wifi
  }
 
  int Connect_time = 0;                       //用于连接计时，如果长时间连接不成功，复位设备
  while (WiFi.status() != WL_CONNECTED)       //等待WIFI连接成功
  {  
    Serial.print(".");                        //一共打印30个点点
    // digitalWrite(LED, !digitalRead(LED));     
    delay(500);
    Connect_time ++;
                                       
    if (Connect_time > 2 * timeOut_s)         //长时间连接不上，重新进入配网页面
    { 
      // digitalWrite(LED, LOW);
      Serial.println("");                     //主要目的是为了换行符
      Serial.println("WIFI autoconnect fail, start AP for webconfig now...");
      wifiConfig();                           //开始配网功能

      wifi_connect_fail = 1;
      return;                                 //跳出 防止无限初始化
    }

    if(test_mode_flag==1){            //测试模式直接退出

      return;
    }

  }
  
  if (WiFi.status() == WL_CONNECTED)          //如果连接成功
  {
    savewificonfig();

    Serial.println("WIFI connect Success");
    Serial.printf("SSID:%s", WiFi.SSID().c_str());
    Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
    Serial.print("LocalIP:");
    Serial.print(WiFi.localIP());
    Serial.print(" ,GateIP:");
    Serial.println(WiFi.gatewayIP());

    Serial.print("KlipperIP:");
    String str(wificonf.klipperip);
    klipper_ip = wificonf.klipperip;
    Serial.println(klipper_ip);

    Serial.print("WIFI status is:");
    Serial.print(WiFi.status());

    // digitalWrite(LED, HIGH);
    server.stop();                            //停止开发板所建立的网络服务器。

    wifi_connect_ok = 1; //已连接上wifi,切换显示
  }
}
 
/*
 * 配置配网功能
 */
void wifiConfig() 
{
  initSoftAP();   
  initDNS();        
  initWebServer();  
  // scanWiFi();       
}

/*
 * 测试时使用
 */
void wifiConfig_test() 
{
  initSoftAP();   
  initDNS();        
  initWebServer();  
  scanWiFi();       
}
 
/*
 * 删除保存的wifi信息，这里的删除是删除存储在flash的信息。删除后wifi读不到上次连接的记录，需重新配网
 */
void restoreWiFi() {
  delay(500);
  esp_wifi_restore();  //删除保存的wifi信息
  Serial.println("连接信息已清空,准备重启设备..");
  delay(10);
  // blinkLED(LED, 5, 500); //LED闪烁5次         //关于LED，不需要可删除 
  // digitalWrite(LED, LOW);                    //关于LED，不需要可删除
}

//wifi ssid，psw保存到eeprom
void savewificonfig()
{
  //开始写入
  uint8_t *p = (uint8_t*)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++)
  {
    EEPROM.write(i + wifi_addr, *(p + i)); //在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit();//执行写入ROM
  delay(10);
}

//删除原有eeprom中的信息
void deletewificonfig()
{
  config_type deletewifi ={{""},{""},{""},{"5"}};//5 AP模式标志位 ， 8 STA模式
  uint8_t *p = (uint8_t*)(&deletewifi);
  for (int i = 0; i < sizeof(deletewifi); i++)
  {
    EEPROM.write(i + wifi_addr, *(p + i)); //在闪存内模拟写入
  }
  delay(10);
  EEPROM.commit();//执行写入ROM
  delay(10);
}

//从eeprom读取WiFi信息ssid，psw
void readwificonfig()
{
  uint8_t *p = (uint8_t*)(&wificonf);
  for (int i = 0; i < sizeof(wificonf); i++)
  {
    *(p + i) = EEPROM.read(i + wifi_addr);
  }
  // EEPROM.commit();
  // Serial.printf("Read WiFi Config.....\r\n");
  // Serial.printf("SSID:%s\r\n",wificonf.stassid);
  // Serial.printf("PSW:%s\r\n",wificonf.stapsw);
  // Serial.printf("KlipperIP:%s\r\n",wificonf.klipperip);
  // Serial.printf("Connecting.....\r\n");

}

/*
 * 检查wifi是否已经连接
 */
void checkConnect(bool reConnect) 
{
  if (WiFi.status() != WL_CONNECTED)           //wifi连接失败
  {
    // if (digitalRead(LED) != LOW) 
    //   digitalWrite(LED, LOW);
    if (reConnect == true && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA ) 
    {
      Serial.println("WIFI未连接.");
      Serial.println("WiFi Mode:");
      Serial.println(WiFi.getMode());
      Serial.println("正在连接WiFi...");
      connectToWiFi(connectTimeOut_s);          //连接wifi函数 
    }
  } 
  // else if (digitalRead(LED) != HIGH)  
  //   digitalWrite(LED, HIGH);                    //wifi连接成功
}
 
/*
 * LED闪烁函数        //用不上LED可删除
 */
// void blinkLED(int led, int n, int t) 
// {
//   for (int i = 0; i < 2 * n; i++) 
//   {
//     digitalWrite(led, !digitalRead(led));
//     delay(t);
//   }
// }
 
 
/*
 * LED初始化
 */
// void LEDinit()
// {
//   pinMode(LED, OUTPUT);                 //配置LED口为输出口
//   digitalWrite(LED, LOW);               //初始灯灭
// }
 
/*
 * 检测客户端DNS&HTTP请求
 */
void checkDNS_HTTP()
{
  dnsServer.processNextRequest();   //检查客户端DNS请求
  server.handleClient();            //检查客户端(浏览器)http请求
}
 