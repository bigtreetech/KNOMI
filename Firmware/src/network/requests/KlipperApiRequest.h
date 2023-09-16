#pragma once
#include "AsyncHTTPRequest_Generic.hpp"
#include "ArduinoJson.h"

class KlipperApiRequest {
private:
  AsyncHTTPRequest request;

protected:
  int failCount = 0;

  virtual String getUrl(String klipper_ip) = 0;
  virtual void processJson(DynamicJsonDocument &doc) = 0;

public:
  int getFailCount() { return failCount; }

  KlipperApiRequest() {
    request.onReadyStateChange([&](void *optParm, AsyncHTTPRequest* req, int readyState) {
             ((KlipperApiRequest *)optParm)->requestCB(req, readyState);
           }, this);
  }

  void requestCB(AsyncHTTPRequest *asyncHttpRequest, int readyState) {
    if (readyState == readyStateDone) {
      if (asyncHttpRequest->responseHTTPcode() == 200) {
        String payload = asyncHttpRequest->responseText();
        DynamicJsonDocument doc(payload.length() * 2);
        deserializeJson(doc, payload);
        failCount = 0;
        processJson(doc);
      } else {
        LV_LOG_INFO("Error on HTTP asyncHttpRequest");
        failCount++;
      }
    }
  }

  void Execute(String& klipper_ip) {
    LV_LOG_INFO("Executing request, state is %d", request.readyState());
    if (request.readyState() == readyStateUnsent ||
        request.readyState() == readyStateDone) {
      if (request.open("GET", getUrl(klipper_ip).c_str())) {
        request.send();
      }
    }
  }
};