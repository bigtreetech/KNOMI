#pragma once
#include "esp_http_server.h"

class KnomiWebServer;

class AbstractPage {
private:
  httpd_uri_t handlerInfo{};

  static int findString(const char *buf, uint size, const char *string) {
    int foundPos = -1;
    size_t stringLength = strlen(string);
    if (size < stringLength)
      return foundPos;

    for (int i = 0; i < size - stringLength; i++) {
      if (strncmp(buf + i, string, stringLength) == 0) {
        // found boundary;
        foundPos = i;
        break;
      }
    }
    LV_LOG_DEBUG("Searching for %s, found at %i", string, foundPos);
    return foundPos;
  }

  static void btox(char *xp, const char *bb, int n) {
    const char xx[] = "0123456789ABCDEF";
    while (--n >= 0)
      xp[n] = xx[(bb[n >> 1] >> ((1 - (n & 1)) << 2)) & 0xF];
  }

  static inline void dump_buf(const char* buf, int size = 512) {
    char *debug = new char[size * 2 + 1];
    memset(debug, 0, size * 2 + 1);
    btox(debug, buf, size * 2);
    LV_LOG_INFO(debug);

    delete[] debug;
  }

  static uint advanceBuffer(httpd_req_t *req, char *buf, int bufferSize, uint currentPos, uint maxPos) {
    LV_LOG_DEBUG("Advance buffer from %i", currentPos);
    uint copyStart = currentPos;
    uint copyLength = maxPos - copyStart;
    if (copyStart > 0 && copyLength > 0) {
      LV_LOG_DEBUG("Moving buffer from %i - %i bytes", copyStart, copyLength);
      char *newBuf = new char[bufferSize];
      memcpy(newBuf, buf + copyStart, copyLength);
      memcpy(buf, newBuf, copyLength);
      delete[] newBuf;
    }
    int bytesRead;
    do {
      bytesRead = httpd_req_recv(req, buf + copyLength, bufferSize - copyLength);
      LV_LOG_DEBUG("Bytes read %i into offset %i, up to %i", bytesRead, copyLength, bufferSize - copyLength);
      copyLength += bytesRead;
    } while (bytesRead != 0 && copyLength < bufferSize);

#if DEBUG
    dump_buf(buf);
#endif

    LV_LOG_DEBUG("Advanced buffer from %i to %i", currentPos, copyLength);
    return copyLength;
  }

  // trim from end of string (right)
  static inline std::string &rtrim(std::string &s, const char *t) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
  }

  // trim from beginning of string (left)
  static inline std::string &ltrim(std::string &s, const char *t) {
    s.erase(0, s.find_first_not_of(t));
    return s;
  }

  // trim from both ends of string (right then left)
  static inline std::string &trim(std::string &s, const char *t) { return ltrim(rtrim(s, t), t); }

protected:
  static String getHeader(httpd_req_t *req, const String &param) {
    LV_LOG_DEBUG("Getting header %s", param.c_str());
    size_t len = httpd_req_get_hdr_value_len(req, param.c_str());
    char *contentType = new char[len + 1];
    memset(contentType, 0, len + 1);
    httpd_req_get_hdr_value_str(req, param.c_str(), contentType, len + 1);
    String header = String(contentType);
    delete[] contentType;
    LV_LOG_DEBUG("Got %s", contentType);
    return header;
  }

  typedef std::function<void(const char *, uint, uint)> ReadCallback;

  static bool streamReadMultipart(httpd_req_t *req, const std::function<ReadCallback(String, String)> &func) {
    String header = getHeader(req, "Content-Type");

    if (!header.startsWith("multipart/form-data")) {
      httpd_resp_send_500(req);
      return false;
    }

    String boundary = "--" + header.substring(header.indexOf("boundary=") + 9);
    streamReadMultipart(req, boundary, func);
    return true;
  }

  static void streamReadMultipart(httpd_req_t *req, const String &boundary,
                                  const std::function<ReadCallback(String, String)> &func) {
    LV_LOG_INFO("Streaming multipart read");
    int bufferSize = 8192;
    char *buf = new char[bufferSize];

    uint read = advanceBuffer(req, buf, bufferSize, 0, 0);

    ReadCallback currentCallback = nullptr;

    while (read > 0) {
      uint pos = 0;
      // search for boundary
      int boundaryPos = findString(buf, read, boundary.c_str());

      uint size = read;
      if (boundaryPos == 0 || boundaryPos == 1) {
        size = 0;
      }
      if (boundaryPos > 1) {
        size = boundaryPos - 2; // remove \r\n before boundary
      }

      uint advance = boundaryPos >= 0 ? size : min(4096u, size);
      if (currentCallback != nullptr) {
        currentCallback(buf, 0, advance);
      }
      pos += advance;

      if (boundaryPos >= 0) {
        LV_LOG_DEBUG("Found boundary at %i", boundaryPos);
        currentCallback = nullptr;
        // need to read headers. Advance buffer;
        read = advanceBuffer(req, buf, bufferSize, boundaryPos + boundary.length(), read);
        pos = 0;

        int posDisp = findString(buf, read, "Content-Disposition:");
        int posDispEnd = findString(buf + posDisp, read - posDisp, "\r\n");
        int content = findString(buf, read, "\r\n\r\n");
        LV_LOG_DEBUG("CD poses %i %i %i", posDisp, posDispEnd, content);
        if (posDisp >= 0) {
          pos = content + 4;

          auto pair = parseContentDisposition(buf, posDisp, posDispEnd);
          String name = String(pair.first.c_str());
          String filename = pair.second.empty() ? "" : String(pair.second.c_str());

          if (!name.isEmpty()) {
            currentCallback = func(name, filename);
            LV_LOG_DEBUG("For %s / %s currentCallback is %i", name.c_str(), filename.c_str(),
                        currentCallback == nullptr ? 0 : 1);
          }
        }
      }

      read = advanceBuffer(req, buf, bufferSize, pos, read);
    }
    delete[] buf;
  }

  static std::pair<std::string, std::string> parseContentDisposition(const char *buf, int posDisp, int posDispEnd) {
    std::string contentDisposition = std::string(buf + posDisp, posDispEnd);
    contentDisposition = contentDisposition.substr(contentDisposition.find(':') + 1);
    LV_LOG_DEBUG("CD = %s", contentDisposition.c_str());
    std::string name;
    std::string filename;
    do {
      auto end = contentDisposition.find(';');
      std::string currentPart;
      LV_LOG_DEBUG("End = %i, Current = %s", end, contentDisposition.c_str());
      if (end != std::string::npos) {
        currentPart = contentDisposition.substr(0, end);
        contentDisposition = contentDisposition.substr(end + 1);
      } else {
        currentPart = contentDisposition;
        contentDisposition = "";
      }
      LV_LOG_DEBUG("CP = %s, CD = %s", currentPart.c_str(), contentDisposition.c_str());

      auto equalsPos = currentPart.find('=');
      if (equalsPos != std::string::npos) {
        std::string key = currentPart.substr(0, equalsPos);
        std::string value = currentPart.substr(equalsPos + 1);
        key = trim(key, " ");
        value = trim(value, "\"");

        LV_LOG_DEBUG("Key = '%s', Value = '%s'", key.c_str(), value.c_str());

        if (key == "name") {
          name = value;
        }
        if (key == "filename") {
          filename = value;
        }
      }

    } while (!contentDisposition.empty());

    return {name, filename};
  }

  static ReadCallback readString(String *result) {
    result->clear();
    ReadCallback reader = [result](const char *buf, int idx, int size) {
      result->concat(buf + idx, size);
    };
    return reader;
  }

protected:
  explicit AbstractPage(httpd_handle_t server, http_method method, const char *path) {
    bool isWebsocket = String("/ws").equals(path);
    handlerInfo = {
        .uri = path,
        .method = method,
        .handler = handlerStatic,
        .user_ctx = this,
        .is_websocket = isWebsocket,
    };

    LV_LOG_INFO("Registering %s %s", http_method_str(method), path);
    esp_err_t i = httpd_register_uri_handler(server, &handlerInfo);
    if (i != ESP_OK) {
      LV_LOG_WARN("Failed to register uri %s: %s", path, esp_err_to_name(i));
    }
  }

  static esp_err_t handlerStatic(httpd_req_t *request) { return ((AbstractPage *)request->user_ctx)->handler(request); }

  virtual esp_err_t handler(httpd_req_t *request) = 0;
};
