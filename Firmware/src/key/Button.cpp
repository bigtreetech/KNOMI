#include "Button.h"
#include "WiFiUser.h"
#include "lvgl.h"

Button::Button() {
  pinMode(0, INPUT); // input mode; default -> high, pressed -> low.
  delay(200);
  currentState = HIGH;
  lastState = HIGH;
}

void Button::KeyScan() {
  currentState = (uint8_t)digitalRead(0); // RESET_PIN

  if (lastState == HIGH && currentState == LOW) // button is pressed
    pressedTime = millis();
  else if (lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

    unsigned long pressDuration = releasedTime - pressedTime;

    if (pressDuration > WIFI_RESET_LONG_PRESS_MS) {
      LV_LOG_INFO("Clearing wifi setup");
      deletewificonfig(); // 删除EEROM保存的wifi信息
      restoreWiFi();      // 删除保存的wifi信息
      ESP.restart();      // 重启复位esp32
    }
  }
  lastState = currentState;
}

bool Button::isPressed() const { return lastState == LOW; }
