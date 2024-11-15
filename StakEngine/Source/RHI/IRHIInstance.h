#pragma once

#include "Types.h"

#include <string>

class IRHIInstance {
public:
  virtual ~IRHIInstance() = default;

  static TScope<IRHIInstance> Create(const std::string &AppName);
};
