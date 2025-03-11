#pragma once

#include "RHIResource.h"

struct FRHIOffscreenRenderTargetDescription {
    FUInt32 Width;
    FUInt32 Height;
    ERHIFormat Format;
    bool UseForImGui;
};

struct IRHITexture : public IRHIResource {
    virtual bool Init(FRHIOffscreenRenderTargetDescription *Description) = 0;

    void Shutdown() override = 0;

    virtual void AddToImGuiWindow() = 0;

    ERHIFormat Format = ERHIFormat::UNDEFINED;
    FRHIRenderArea RenderArea = {};
};
