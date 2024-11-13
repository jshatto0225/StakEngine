#pragma once

#include "Types.h"

namespace Stak {

enum class ERHIShaderStage {
  NONE,
  VERTEX,
  FRAGMENT,
};

struct RHIShaderDescription {

};

class IRHIShader {
public:
  virtual ~IRHIShader() = default;
};

} // namespace Stak