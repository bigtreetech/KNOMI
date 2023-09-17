#pragma once

#include <WString.h>
#include <lvgl.h>

class BaseConfig {
protected:
  String configNamespace;
  virtual void load() = 0;

public:
  virtual void save() = 0;
  String getConfigNamespace() { return this->configNamespace; }
};