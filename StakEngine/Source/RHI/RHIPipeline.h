#pragma once

#include "RHIDescriptorSetLayout.h"
#include "RHIResource.h"
#include "RHIBuffer.h"
#include "RHIShader.h"

struct FRHIPipelineLayoutDescription {
    std::vector<FRHIDescriptorSetLayout> DescriptorSetLayouts;
};

class IRHIPipelineLayout {
public:
    virtual void Shutdown() = 0;
};

class FRHIPipelineLayout {
public:
    FRHIPipelineLayout(TRef<IRHIPipelineLayout> Layout);
    
    TRef<IRHIPipelineLayout> GetImpl();
    
    void Shutdown();
    
private:
    TRef<IRHIPipelineLayout> Impl;
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

struct FRHIPipelineStateDescription {
    std::vector<ERHIFormat> ColorFormats;
    ERHIFormat DepthStencilFormat;
    std::vector<FRHIShader *> Shaders;
    std::vector<FRHIVertexInputBinding> VertexInputBindings;
    std::vector<FRHIBufferElement> VertexInputAttributes;
    FRHIPipelineLayout *Layout;
};

class IRHIPipeline {
public:
    virtual void Shutdown() = 0;
};

class FRHIPipeline {
public:
    FRHIPipeline(TRef<IRHIPipeline> Pipeline);
    
    void Shutdown();
    
private:
    TRef<IRHIPipeline> Impl = nullptr;
};
