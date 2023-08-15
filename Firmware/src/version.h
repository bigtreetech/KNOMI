#pragma once
#include <Arduino.h>
class Version {
public:
  static String getGitCommitSha1();
  static String getBuildTimestamp();
};