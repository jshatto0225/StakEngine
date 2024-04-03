#pragma once

#include "Types.h"

struct framebuffer_spec
{
  u32 Width;
  u32 Height;
  u32 Samples;
  bool SwapChainTarget;
};

struct framebuffer;

framebuffer *CreateFramebuffer(const framebuffer_spec *Spec);
void DestroyFramebuffer(framebuffer **Framebuffer);
void ResizeFramebuffer(framebuffer *Framebuffer, const framebuffer_spec *Spec);
void BindFramebuffer(const framebuffer *Framebuffer);
void UnbindFramebuffer(const framebuffer *Framebuffer);