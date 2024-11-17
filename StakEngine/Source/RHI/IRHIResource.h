#pragma once

#include "RHICore.h"

class IRHIBuffer {

};

class IRHITexture {
public:
  static TRef<IRHITexture> Create(TRef<IRHIDevice> Device, FRHITextureDescription &Description);

  virtual ~IRHITexture() = default;

  virtual FUInt32 GetWidth() = 0;
  virtual FUInt32 GetHeight() = 0;
  virtual FUInt32 GetLayers() = 0;
  virtual const FRHITextureDescription &GetDescription() = 0;
};
