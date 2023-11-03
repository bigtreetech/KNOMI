#pragma once
#include "ArduinoJson.h"
#include "StreamString.h"
#include "WString.h"

// conflicts with ESPAsyncWebServer
namespace esp {
#include "esp_http_client.h"
#include "esp_tls.h"
} // namespace esp

using namespace esp;

class KlipperApiRequest {
private:
  String response = "";
  bool inProgress = false;

protected:
  int failCount = 0;

  virtual const char* getUrl() = 0;
  virtual void processJson(JsonDocument &doc) = 0;

public:
  int getFailCount() const { return failCount; }

  KlipperApiRequest() {

  }

  void Execute(String &klipper_ip) {
    if (inProgress) {
      return;
    }

    inProgress = true;
    response = "";

    const char *path = getUrl();
    LV_LOG_INFO("Http request to %s %s", klipper_ip.c_str(), path);

    esp_http_client_config_t config = {
        .host = klipper_ip.c_str(),
        .path = path,
        .disable_auto_redirect = true,
        .event_handler = _http_event_handler,
        .user_data = this
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
      LV_LOG_INFO("HTTP GET Status = %d, content_length = %i", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    } else {
      LV_LOG_INFO("HTTP GET request failed: %s", esp_err_to_name(err));
    }
    inProgress = false;
    esp_http_client_cleanup(client);
  }

  static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    auto kar = (KlipperApiRequest *)evt->user_data;
    return kar->httpEventHandler(evt);
  }

  esp_err_t httpEventHandler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      LV_LOG_WARN("HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      // Incoming order: 1
      LV_LOG_INFO("HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      // Incoming order: 2
      LV_LOG_INFO("HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      // Incoming order: 3. Good place to save headers.
      LV_LOG_INFO("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      // Incoming order: 4. Aggregate data
      LV_LOG_INFO("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      char *buffer;
      buffer = new char[evt->data_len + 1];
      memset(buffer, 0, evt->data_len + 1);
      memcpy(buffer, evt->data, evt->data_len);
      response += String(buffer, evt->data_len);
      delete[] buffer;
      break;
    case HTTP_EVENT_ON_FINISH:
      // Incoming order: 5. Request fully buffered locally. process
      LV_LOG_INFO("HTTP_EVENT_ON_FINISH");
      if (esp_http_client_get_status_code(evt->client) == 200) {
        StaticJsonDocument<2048> doc;
        deserializeJson(doc, response.c_str());
        failCount = 0;
        unsigned int bufLen = response.length() * 2;
        auto buf = new char[bufLen];
        serializeJson(doc, buf, bufLen);
        LV_LOG_INFO("Parsed response:");
        LV_LOG_INFO(buf);
        delete[] buf;
        processJson(doc);
      } else {
        LV_LOG_INFO("Error on HTTP asyncHttpRequest");
        failCount++;
      }

      response = "";
      break;
    case HTTP_EVENT_DISCONNECTED: {
      LV_LOG_INFO("HTTP_EVENT_DISCONNECTED");
      int mbedtls_err = 0;
      esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
      if (err != 0) {
        LV_LOG_INFO("Last esp error code: 0x%x", err);
        LV_LOG_INFO("Last mbedtls failure: 0x%x", mbedtls_err);
      }
    } break;
    default:
      LV_LOG_INFO("Unhandled eventId %i", evt->event_id);
      break;
    }
    yield();
    return ESP_OK;
  }
};