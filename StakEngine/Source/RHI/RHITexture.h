#pragma once

#include "RHIResource.h"

class IRHITexture : public IRHIResource {
public:
    void Shutdown() override = 0;

    virtual bool IsBackbuffer() = 0;

    virtual ERHIFormat GetFormat() = 0;

    inline ERHIResourceType GetType() override { return ERHIResourceType::TEXTURE; }

    virtual FRHIRenderArea GetRenderArea() = 0;
};
