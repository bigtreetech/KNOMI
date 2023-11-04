#pragma once
#include <string>

void logToSerial(const char *logLevel, const char *file, int line, const char *func, const char *format, ...);

#ifdef DEBUG
#define LV_LOG_DEBUG(...) logToSerial("DEBUG", __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LV_LOG_DEBUG(...) {}
#endif

#define LV_LOG_INFO(...) logToSerial("INFO", __FILE__, __LINE__, __PRETTY_FUNCTION__ , __VA_ARGS__)
#define LV_LOG_WARN(...) logToSerial("WARN", __FILE__, __LINE__, __PRETTY_FUNCTION__ , __VA_ARGS__)
