#pragma once

#include "RHIShader.h"
#include <vector>

enum class ERHIDescriptorType {
    UNIFORM_BUFFER,
    TEXTURE,
};

struct FRHIDescriptorBinding {
    FUInt32 Binding;
    ERHIDescriptorType Type;
    std::vector<ERHIShaderType> ShaderStages;
    FUInt32 Count;
};

struct FRHIDescriptorSetLayoutDescription {
    std::vector<FRHIDescriptorBinding> Bindings;
};

class IRHIDescriptorSetLayout {
public:
    virtual bool Init(const FRHIDescriptorSetLayoutDescription &Description) = 0;
    virtual void Shutdown() = 0;
};
