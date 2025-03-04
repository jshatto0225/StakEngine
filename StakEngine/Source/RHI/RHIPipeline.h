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
    FRHIPipelineLayout() = default;

public:
    FRHIPipelineLayout(TRef<IRHIPipelineLayout> Layout);
    
    TRef<IRHIPipelineLayout> GetImpl() const { return Impl; }
    
    void Shutdown();
    
private:
    TRef<IRHIPipelineLayout> Impl;
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
    FRHIPipeline() = default;

public:
    FRHIPipeline(TRef<IRHIPipeline> Pipeline);

    inline TRef<IRHIPipeline> GetImpl() const { return Impl; }
    
    void Shutdown();
    
private:
    TRef<IRHIPipeline> Impl = nullptr;
};
