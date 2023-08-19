#pragma once
#include "../network/WifiConfig.h"
#include <Arduino.h>

class Button final {

public:
  Button(WifiConfig* wifiConfig);
  void KeyScan();

  bool isPressed() const;

private:
  WifiConfig* wificonfig;

  const int WIFI_RESET_LONG_PRESS_MS = 4000; // long press for ~4 sec

  int lastState = LOW;  // the previous state from the input pin
  int currentState;     // the current reading from the input pin

  unsigned long pressedTime  = 0;
  unsigned long releasedTime = 0;
};