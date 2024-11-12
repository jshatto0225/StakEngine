#pragma once

#include "Types.h"
#include "RHICore.h"
#include "IRHIShader.h"

namespace Stak {

// https://alextardif.com/RenderingAbstractionLayers.html

enum ERHIBufferType {
  VERTEX,
  INDEX,
  UNIFORM,
};

struct RHIBufferElement {
  ERHIDataFormat format;
  u32 byteOffset;
};

struct RHIBufferDescription {
  ERHIBufferType type;
  std::vector<RHIBufferElement> elements;
  u32 count;
  ERHIShaderStage stage;
};

class IRHIBuffer {
public:
  virtual ~IRHIBuffer() = default;

  virtual u32 getStride() = 0;
  virtual u32 getSize() = 0;
  virtual u32 getNumElements() = 0;
};

} // namespace Stak