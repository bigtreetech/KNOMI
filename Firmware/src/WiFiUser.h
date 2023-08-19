#pragma once
#include "network/KnomiWebServer.h"
#include "network/WifiConfig.h"
#include <DNSServer.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <esp_wifi.h> //用于esp_wifi_restore() 删除保存的wifi信息

extern const char *HOST_NAME; // 设置设备名

//===========需要调用的函数===========
void checkConnect(bool reConnect); // 检测wifi是否已经连接
void restoreWiFi();                // 删除保存的wifi信息
void checkDNS_HTTP();              // 检测客户端DNS&HTTP请求
void connectToWiFi(); // 连接WiFi

//===========内部函数===========
void initSoftAP();       // 进入AP模式
void initDNS();          // 开启DNS服务器
void wifiConfig(); // 配置配网功能
