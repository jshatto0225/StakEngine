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

struct IRHIShader {
    virtual bool Init(FRHIShaderDescription *Description) = 0;

    virtual void Shutdown() = 0;

    ERHIShaderType Type = ERHIShaderType::VERTEX;
};
