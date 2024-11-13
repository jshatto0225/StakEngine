#pragma once

#include "Types.h"
#include "IRHIShader.h"
#include "RHICore.h"

namespace Stak {

enum class ERHIAttachmentType {
  NONE,
  COLOR,
  DEPTH,
  STENCIL,
  RESOLVE
};

enum class ERHITextureUsage {
  NONE,
  SAMPLED,
  STORAGE,
  TRANSFER_SRC,
  TRANSFER_DST,
};

struct RHITextureDescription {
  i32 width;
  i32 height;
  ERHIFormat dataFormat;
  ERHIShaderStage stage;
  ERHITextureUsage usage;
  ERHIAttachmentType attachmentType;
};

class IRHITexture {
public:
  virtual ~IRHITexture() = default;

  struct TextureSizeData {
    i32 x;
    i32 y;
  };

  virtual TextureSizeData getSize() = 0;
  virtual void setSize(i32 x, i32 y) = 0;
  virtual void setUsage(ERHITextureUsage usage) = 0;
  virtual ERHITextureUsage getUsage() = 0;
};

} // namespace Stak