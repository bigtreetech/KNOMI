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

  struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
    int bufLen;
    char *buf;
  };

  static void ws_async_send(void *arg) {
    auto *resp_arg = (async_resp_arg *)arg;

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    ws_pkt.len = resp_arg->bufLen;
    ws_pkt.payload = (uint8_t *)resp_arg->buf;

    httpd_ws_send_frame_async(resp_arg->hd, resp_arg->fd, &ws_pkt);
    delete resp_arg->buf;
    delete resp_arg;
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
        auto *resp_arg = new async_resp_arg();
        resp_arg->hd = server;
        resp_arg->fd = client_fds[i];
        resp_arg->bufLen = strlen(string);
        resp_arg->buf = new char[resp_arg->bufLen + 1];
        resp_arg->buf[resp_arg->bufLen] = 0;
        memcpy(resp_arg->buf, (void *)string, resp_arg->bufLen);

        esp_err_t ret = httpd_queue_work(server, ws_async_send, resp_arg);
        if (ret != ESP_OK) {
          delete resp_arg->buf;
          delete resp_arg;
        }
      }
    }
  }
};