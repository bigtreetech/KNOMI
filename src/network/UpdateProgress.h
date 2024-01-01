#pragma once

struct UpdateProgress {
  bool isInProgress;

  int current;
  int total;

  bool canStartUpdate;

  bool waitForCanWrite() const {
    const int now = millis();
    while (!canStartUpdate || ((millis() - now) > 500)) {
      vTaskDelay(25);
    }
    return canStartUpdate;
  }
};