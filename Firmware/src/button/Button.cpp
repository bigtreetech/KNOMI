#include "Button.h"

Button::Button(WifiManager* manager) {
  wifiManager = manager;
  pinMode(0, INPUT); // input mode; default -> high, pressed -> low.
  delay(200);
  currentState = HIGH;
  lastState = HIGH;

  timer.attach(0.001, Button::KeyScan, this);
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
      wifiManager->resetWifi();
      ESP.restart();
    }
  }
  lastState = currentState;
}

bool Button::isPressed() const { return lastState == LOW; }
