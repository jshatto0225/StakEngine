#pragma once

#include <array>

#include "Types.h"

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() {}

    virtual void Init() = 0;
    virtual void Shutdown() = 0;
    
    virtual void Draw() = 0;
    virtual void Clear() = 0;
    virtual void SetClearColor() = 0;

    virtual bool IsRunning() { return mRunning; };

protected:
    std::array<f32, 4> clearColor;
    bool mRunning;
};
