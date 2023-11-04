#pragma once
#include "AbstractPage.h"

class WebsocketLog : public AbstractPage {
public:
  explicit WebsocketLog(httpd_handle_t server) : AbstractPage(server, HTTP_GET, "/ws") {}

  esp_err_t handler(httpd_req_t *req) override {
    /* TODO
    auto *pSocket = new AsyncWebSocket("/ws");
    pSocket->onEvent([&](AsyncWebSocket *_unused, AsyncWebSocketClient *client, AwsEventType type, void *arg,
                         uint8_t *data, size_t len) {
      switch (type) {
      case WS_EVT_CONNECT:
        LV_LOG_INFO("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        LV_LOG_INFO("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        // Here is an example on how to handle message from JS. Probably we won't
        // ever need that.
        // AwsFrameInfo *info = (AwsFrameInfo*)arg;
        // if (info->final && info->index == 0 && info->len == len && info->opcode
        // == WS_TEXT) { data[len] = 0; String message = (char*)data;
        // Check if the message is "getReadings"
        // if (strcmp((char*)data, "getReadings") == 0) {
        // if it is, send current sensor readings
        // String sensorReadings = getSensorReadings();
        // notifyClients(sensorReadings);
        //}
        //}
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
      }
    });
    pServer->addHandler(pSocket);
     */
    return ESP_OK;
  }
  void textAll(const char *string) {
    // TODO
    // if (socket != nullptr && !socket->getClients().isEmpty()) {
    //   socket->textAll(logString);
    // }
  }
};