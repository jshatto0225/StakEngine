#pragma once

#include "RHIDescriptorSetLayout.h"
#include "RHIResource.h"
#include "RHIBuffer.h"
#include "RHIShader.h"

struct FRHIPipelineLayoutDescription {
    std::vector<TRef<IRHIDescriptorSetLayout>> DescriptorSetLayouts;
};

class IRHIPipelineLayout {
public:
    virtual bool Init(const FRHIPipelineLayoutDescription &Description) = 0;
    virtual void Shutdown() = 0;
};

enum class ERHIPipelineType {
    GRAPHICS
};

enum class ERHIVertexInputRate {
    PER_VERTEX,
    PER_INSTANCE,
};

struct FRHIVertexInputBinding {
    FUInt32 Binding;
    FUInt32 Stride;
    ERHIVertexInputRate InputRate;
};

struct FRHIGraphicsPipelineStateDescription {
    std::vector<ERHIFormat> ColorFormats;
    ERHIFormat DepthStencilFormat;
    std::vector<TRef<IRHIShader>> Shaders;
    std::vector<FRHIVertexInputBinding> VertexInputBindings;
    std::vector<FRHIBufferElement> VertexInputAttributes;
    TRef<IRHIPipelineLayout> Layout;
};

class IRHIPipeline {
public:
    virtual bool Init(const FRHIGraphicsPipelineStateDescription &Description) = 0;
    virtual void Shutdown() = 0;
};
