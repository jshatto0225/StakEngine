#pragma once

#include "RHIDescriptorSetLayout.h"
#include "RHIPipeline.h"
#include "Window.h"

#include "RHIForward.h"

class IRHIViewport {
public:
    virtual TRef<IRHITexture> GetBackbuffer() = 0;

    virtual void OnFramebufferResize() = 0;

    virtual void Shutdown() = 0;

protected:
    virtual ~IRHIViewport() = default;
};
