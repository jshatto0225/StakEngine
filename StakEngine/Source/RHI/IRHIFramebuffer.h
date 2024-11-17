#pragma once

#include "RHICore.h"

class IRHIFramebuffer {
public:
  virtual ~IRHIFramebuffer() = default;

  static TRef<IRHIFramebuffer> Create(TRef<IRHIDevice> Device, std::vector<TRef<IRHITexture>> Textures, TRef<IRHIRenderPass> RenderPass);
};