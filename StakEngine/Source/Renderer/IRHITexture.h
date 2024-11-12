#pragma once

#include "Types.h"
#include "IRHIShader.h"

namespace Stak {

enum class ERHIImageFormat {
  UNORM,
  SRGB
};

enum class ERHITextureType {
  SAMPLER,
  RENDER_TARGET
};

struct RHITextureDescription {
  i32 width;
  i32 height;
  ERHIDataFormat dataFormat;
  ERHIImageFormat imageFormat;
  ERHIShaderStage stage;
};

enum class ETextureUsage {
  None,
  Sampled,
  Storage,
  TransferSrc,
  TransferDst,
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
  virtual void setUsage(ETextureUsage usage) = 0;
  virtual ETextureUsage getUsage() = 0;
};

} // namespace Stak