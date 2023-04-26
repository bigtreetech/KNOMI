#ifndef __WIFIUSER_H__
#define __WIFIUSER_H__
 
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESPmDNS.h>      //用于设备域名 MDNS.begin("esp32")
#include <esp_wifi.h>     //用于esp_wifi_restore() 删除保存的wifi信息
#include <EEPROM.h>

extern const int LED;                         //设置LED引脚
extern const char* HOST_NAME;                 //设置设备名
extern int connectTimeOut_s;                 //WiFi连接超时时间，单位秒
extern String klipper_ip;                     //暂时存储KlipperIP
extern String wifi_ssid;                     //暂时存储wifi账号密码
extern int connectTimeOut_s;                 //WiFi连接超时时间，单位秒

extern String scanNetworksID;                //用于储存扫描到的WiFi ID
extern String scanNetworksID1;
extern String scanNetworksID2;
extern String scanNetworksID3;

struct config_type
{
  char stassid[32];//定义配网得到的WIFI名长度(最大32字节)
  char stapsw[64];//定义配网得到的WIFI密码长度(最大64字节)
  char klipperip[32];//定义配网得到的klipperip长度(最大32字节)  
  char apmodeflag[32];//开机进入ap模式标志位(最大32字节) 
};

extern config_type wificonf;

//===========需要调用的函数===========
void checkConnect(bool reConnect);    //检测wifi是否已经连接
void restoreWiFi();                   //删除保存的wifi信息
void LEDinit();                       //LED初始化
void checkDNS_HTTP();                 //检测客户端DNS&HTTP请求
void connectToWiFi(int timeOut_s);    //连接WiFi
 
//===========内部函数===========
void handleRoot();                    //处理网站根目录的访问请求
void handleConfigWifi() ;             //提交数据后的提示页面
void handleNotFound();                //处理404情况的函数'handleNotFound'
void initSoftAP();                    //进入AP模式
void initDNS();                       //开启DNS服务器
void initWebServer();                 //初始化WebServer
bool scanWiFi();                      //扫描附近的WiFi，为了显示在配网界面
void wifiConfig();                    //配置配网功能
void wifiConfig_test();               //测试时使用
void blinkLED(int led, int n, int t); //LED闪烁函数        //用不上LED可删除

//===========需要保存的数据wifi账号、wifi密码、klipperIp===========
void savewificonfig();
void readwificonfig();
void deletewificonfig(); 
void connection_timing(int Connect_time);
#endif