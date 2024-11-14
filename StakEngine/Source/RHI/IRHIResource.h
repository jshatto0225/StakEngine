#pragma once

#include "RHICore.h"

class IRHIBuffer {

};

class IRHITexture {
public:
  static TRef<IRHITexture> Create(TRef<IRHIDevice> Device, FRHITextureDescription &Description);

  virtual ~IRHITexture() = 0;
};
