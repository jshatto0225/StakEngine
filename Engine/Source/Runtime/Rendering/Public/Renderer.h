#pragma once

#include <array>

#include "Types.h"

class Renderer
{
public:
    virtual void Init() = 0;
    virtual void Draw() = 0;
    virtual void Clear() = 0;
    virtual void SetClearColor() = 0;

protected:
    std::array<f32, 4> clearColor;
};