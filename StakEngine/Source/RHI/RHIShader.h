#pragma once

#include "RHIForward.h"

#include <string>

enum class ERHIShaderType {
    VERTEX,
    FRAGMENT
};

struct FRHIShaderDescription {
    std::string Name;

    ERHIShaderType Type;
};

class IRHIShader {
public:
    virtual ~IRHIShader() = default;
    
    virtual ERHIShaderType GetType() = 0;
    
    virtual void Shutdown() = 0;
};
