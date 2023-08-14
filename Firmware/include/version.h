#pragma once
#include <string>
class Version {
  public:
    static std::string getGitCommitSha1();
    static std::string getBuildTimestamp();
};