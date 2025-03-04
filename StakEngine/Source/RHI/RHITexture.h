#pragma once

#include "RHIResource.h"

class IRHITexture : public IRHIResource {
public:
    void Shutdown() override = 0;

    virtual bool IsBackbuffer() = 0;

    virtual ERHIFormat GetFormat() = 0;

    virtual FRHIRenderArea GetRenderArea() = 0;
};

class FRHITexture : public FRHIResource {
public:
    FRHITexture() = default;

public:
    // NOTE: To be used for swapchain images created by a device
    FRHITexture(TRef<IRHITexture> Texture);

    bool IsBackbuffer();

    FRHIRenderArea GetRenderArea() const;

    ERHIFormat GetFormat();
    
    inline ERHIResourceType GetType() const override {
        return ERHIResourceType::TEXTURE;
    }

    void Shutdown() override;
};
