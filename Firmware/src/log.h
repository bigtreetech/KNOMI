#pragma once
#include <string>

void logToSerial(const char *logLevel, const char *file, int line, const char *func, const char *format, ...);

#define LV_LOG_INFO(...) logToSerial("INFO", __FILE__, __LINE__, __func__, __VA_ARGS__);
#define LV_LOG_WARN(...) logToSerial("WARN", __FILE__, __LINE__, __func__, __VA_ARGS__);
