#pragma once

class SceneTimer {
private:
  int createdAt = millis();
  int duration;

public:
  SceneTimer(int duration) { this->duration = duration; }

  int elapsed() { return millis() - createdAt; }

  bool isCompleted() { return elapsed() >= duration; }
};