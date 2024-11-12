#pragma once

#include "Types.h"

namespace Stak {

struct TextureDescription {

};

class IRHITexture {
public:
  virtual ~IRHITexture() = default;
};

} // namespace Stak