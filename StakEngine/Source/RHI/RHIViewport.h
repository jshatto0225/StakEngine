#pragma once

#include "RHIDescriptorSetLayout.h"
#include "RHIPipeline.h"
#include "Window.h"

#include "RHIForward.h"

struct IRHIViewport {
    virtual bool Init() = 0;

    virtual void Shutdown() = 0;

    bool FramebufferResized = false;
    TRef<IRHITexture> CurrentBackbuffer;
    std::vector<TRef<IRHITexture>> Backbuffers;
};
