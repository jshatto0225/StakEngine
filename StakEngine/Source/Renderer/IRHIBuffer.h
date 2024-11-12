#pragma once

#include "Types.h"
#include "RHICore.h"

namespace Stak {

// https://alextardif.com/RenderingAbstractionLayers.html

struct BufferDescription {

};

class IRHIBuffer {
public:
  virtual ~IRHIBuffer() = default;

  virtual u32 getStride() = 0;
  virtual u32 getSize() = 0;
  virtual u32 getNumElements() = 0;

  virtual void setApiResource(HRHIResource resource) = 0;
  virtual HRHIResource getApiResource() = 0;
};

} // namespace Stak