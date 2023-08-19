#pragma once
#include <Arduino.h>
class Version final {
public:
  static String getGitCommitSha1();
  static String getBuildTimestamp();
};