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

  virtual String getUrl(String klipper_ip) = 0;
  virtual void processJson(JsonDocument &doc) = 0;

public:
  int getFailCount() { return failCount; }

  KlipperApiRequest() {}

  void Execute(String &klipper_ip) {
    if (inProgress)
      return;

    inProgress = true;
    response = "";

    esp_http_client_config_t config = {
        .url = getUrl(klipper_ip).c_str(),
        .disable_auto_redirect = true,
        .event_handler = _http_event_handler,
        .user_data = this, // Pass address of local buffer to get response
        .is_async = true,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
    while (1) {
      err = esp_http_client_perform(client);
      if (err != ESP_ERR_HTTP_EAGAIN) {
        break;
      }
    }
    if (err == ESP_OK) {
      LV_LOG_INFO("HTTP GET Status = %d, content_length = %", esp_http_client_get_status_code(client),
                  esp_http_client_get_content_length(client));
    } else {
      LV_LOG_INFO("HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
  }

  static esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    auto kar = (KlipperApiRequest *)evt->user_data;
    return kar->httpEventHandler(evt);
  }

  esp_err_t httpEventHandler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      LV_LOG_INFO("HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      LV_LOG_INFO("HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      LV_LOG_INFO("HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      LV_LOG_INFO("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      LV_LOG_INFO("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      char *buffer;
      buffer = new char[evt->data_len + 1];
      memset(buffer, 0, evt->data_len + 1);
      memcpy(buffer, evt->data, evt->data_len);
      response += String(buffer, evt->data_len);
      delete[] buffer;
      break;
    case HTTP_EVENT_ON_FINISH:
      LV_LOG_INFO("HTTP_EVENT_ON_FINISH");
      if (esp_http_client_get_status_code(evt->client) == 200) {
        StaticJsonDocument<200> doc;
        deserializeJson(doc, response.c_str());
        failCount = 0;
        processJson(doc);
      } else {
        LV_LOG_INFO("Error on HTTP asyncHttpRequest");
        failCount++;
      }

      response = "";
      break;
    case HTTP_EVENT_DISCONNECTED:
      LV_LOG_INFO("HTTP_EVENT_DISCONNECTED");
      int mbedtls_err = 0;
      esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
      if (err != 0) {
        LV_LOG_INFO("Last esp error code: 0x%x", err);
        LV_LOG_INFO("Last mbedtls failure: 0x%x", mbedtls_err);
      }
      break;
    }
    return ESP_OK;
  }
};