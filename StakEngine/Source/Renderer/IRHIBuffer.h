#pragma once

#include "Types.h"
#include "RHICore.h"
#include "IRHIShader.h"

namespace Stak {

// https://alextardif.com/RenderingAbstractionLayers.html

enum ERHIBufferUsage {
  STAGING,
  VERTEX_DEVICE,
  VERTEX_SHARED,
  INDEX_DEVICE,
  INDEX_SHARED,
  UNIFORM_DEVICE,
  UNIFORM_SHARED
};

struct RHIBufferElement {
  ERHIFormat format;
  u32 byteOffset;
};

struct RHIBufferDescription {
  ERHIBufferUsage usage;
  std::vector<RHIBufferElement> elements;
  u32 count;
  ERHIShaderStage stage;
};

class IRHIBuffer {
public:
  virtual ~IRHIBuffer() = default;

  virtual void setData(u32 size, void *data) = 0;
  virtual u32 getStride() = 0;
  virtual u32 getSize() = 0;
  virtual u32 getNumElements() = 0;
  virtual ERHIBufferUsage getUsage() = 0;
};

} // namespace Stak