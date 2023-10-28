#pragma once
#include "ArduinoJson.h"
#include "AsyncHTTPRequest_Generic.hpp"

class KlipperApiRequest {
private:
  AsyncHTTPRequest request;

protected:
  int failCount = 0;

  virtual String getUrl(String klipper_ip) = 0;
  virtual void processJson(JsonDocument &doc) = 0;

public:
  int getFailCount() { return failCount; }

  KlipperApiRequest() {
    request.onReadyStateChange([&](void *optParm, AsyncHTTPRequest *req,
                                   int readyState) { ((KlipperApiRequest *)optParm)->requestCB(req, readyState); },
                               this);
  }

  void requestCB(AsyncHTTPRequest *asyncHttpRequest, int readyState) {
    if (readyState == readyStateDone) {
      if (asyncHttpRequest->responseHTTPcode() == 200) {
        String payload = asyncHttpRequest->responseText();
        StaticJsonDocument<200> doc;
        deserializeJson(doc, payload.c_str());
        failCount = 0;
        processJson(doc);
      } else {
        LV_LOG_INFO("Error on HTTP asyncHttpRequest");
        failCount++;
      }
    }
  }

  void Execute(String &klipper_ip) {
    LV_LOG_INFO("Executing request, state is %d", request.readyState());
    if (request.readyState() == readyStateUnsent || request.readyState() == readyStateDone) {
      if (request.open("GET", getUrl(klipper_ip).c_str())) {
        request.send();
      }
    }
  }
};