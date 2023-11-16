#pragma once
#include "AbstractPage.h"

class WebsocketLog : public AbstractPage {
private:
  httpd_handle_t server;

public:
  explicit WebsocketLog(httpd_handle_t server) : AbstractPage(server, HTTP_GET, "/ws") { this->server = server; }

  esp_err_t handler(httpd_req_t *req) override {
    if (req->method == HTTP_GET) {
      ESP_LOGI(TAG, "Handshake done, the new connection was opened");
      return ESP_OK;
    }

    return ESP_OK;
  }

  void textAll(const char *string) {
    static constexpr size_t max_clients = CONFIG_LWIP_MAX_LISTENING_TCP;
    size_t fds = max_clients;
    int client_fds[max_clients] = {0};

    esp_err_t ret = httpd_get_client_list(server, &fds, client_fds);

    if (ret != ESP_OK) {
      return;
    }

    for (int i = 0; i < fds; i++) {
      auto client_info = httpd_ws_get_fd_info(server, client_fds[i]);
      if (client_info == HTTPD_WS_CLIENT_WEBSOCKET) {
        httpd_ws_frame_t ws_pkt;
        memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
        ws_pkt.type = HTTPD_WS_TYPE_TEXT;
        ws_pkt.len = strlen(string);
        ws_pkt.payload = (uint8_t *)string;

        httpd_ws_send_frame_async(server, client_fds[i], &ws_pkt);
      }
    }
  }
};