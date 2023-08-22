#pragma once
#include <DNSServer.h>
#include "lvgl.h"

class DnsService {
private:
  DNSServer dnsServer;       // 创建dnsServer实例
  const byte DNS_PORT = 53; // 设置DNS端口号

public:
  DnsService(IPAddress apIP) {
    if (dnsServer.start(DNS_PORT, "*",
                        apIP)) // 判断将所有地址映射到esp32的ip上是否成功
    {
      LV_LOG_INFO("start dnsserver success.");
    } else {
      LV_LOG_INFO("start dnsserver failed.");
    }
  }

  ~DnsService() {
    dnsServer.stop();
  }

  void tick() {
    dnsServer.processNextRequest(); // 检查客户端DNS请求
  }
};