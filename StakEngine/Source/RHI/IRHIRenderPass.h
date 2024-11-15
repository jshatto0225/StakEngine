#pragma once

#include "RHICore.h"

class IRHIRenderPass {
public:
  virtual ~IRHIRenderPass() = 0;

  virtual void SetFramebuffer(TRef<IRHIFramebuffer> Framebuffer) = 0;
};
