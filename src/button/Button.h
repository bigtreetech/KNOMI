#pragma once
#include "../config/Config.h"
#include "./network/WifiManager.h"
#include <Arduino.h>
#include <Ticker.h>

class Button final {

public:
  explicit Button(WifiManager *mgr, Config *config);
  void KeyScan();

  static void KeyScan(Button *button) { button->KeyScan(); }

  bool isPressed() const;

private:
  Config *config;
  WifiManager *manager;
  Ticker timer;

  const int WIFI_RESET_LONG_PRESS_MS = 4000; // long press for ~4 sec

  int lastState = LOW; // the previous state from the input pin
  int currentState;    // the current reading from the input pin

  unsigned long pressedTime = 0;
  unsigned long releasedTime = 0;
};