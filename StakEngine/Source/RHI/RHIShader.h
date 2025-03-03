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

class FRHIShader {
public:
    FRHIShader(TRef<IRHIShader> Shader);
    
    ERHIShaderType GetType();
    
    inline TRef<IRHIShader> GetImpl() const { return Impl; }
    
    void Shutdown();
    
private:
    TRef<IRHIShader> Impl = nullptr;
};
