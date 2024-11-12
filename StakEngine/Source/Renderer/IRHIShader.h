#pragma once

#include "Types.h"

namespace Stak {

enum class ERHIShaderStage {
  VERTEX,
  FRAGMENT
};

struct RHIShaderDescription {

};

class IRHIShader {
public:
  virtual ~IRHIShader() = default;
};

} // namespace Stak