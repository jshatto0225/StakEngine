#pragma once

#include "RHICore.h"

class IRHIFramebuffer {
public:
  virtual ~IRHIFramebuffer() = default;

  TRef<IRHIFramebuffer> Create(TRef<IRHIDevice> Device, std::vector<TRef<IRHITexture>> Textures);
};