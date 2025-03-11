#pragma once

#include "RHIResource.h"

struct FRHIOffscreenRenderTargetDescription {
    FUInt32 Width;
    FUInt32 Height;
    ERHIFormat Format;
    bool UseForImGui;
};

class IRHITexture : public IRHIResource {
public:
    virtual bool Init(FRHIOffscreenRenderTargetDescription *Description) = 0;

    void Shutdown() override = 0;

    virtual bool IsSwapchainBackbuffer() = 0;

    virtual ERHIFormat GetFormat() = 0;

    inline ERHIResourceType GetType() override { return ERHIResourceType::TEXTURE; }

    virtual FRHIRenderArea GetRenderArea() = 0;

    virtual void AddToImGuiWindow() = 0;
};
