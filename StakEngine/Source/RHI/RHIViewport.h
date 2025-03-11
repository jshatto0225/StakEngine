#pragma once

#include "RHIDescriptorSetLayout.h"
#include "RHIPipeline.h"
#include "Window.h"

#include "RHIForward.h"

class IRHIViewport {
public:
    virtual bool Init() = 0;

    virtual TRef<IRHITexture> GetCurrentBackbuffer() = 0;

    virtual void OnFramebufferResize() = 0;

    virtual void Shutdown() = 0;
};
