#pragma once

#include <array>

#include "Types.h"
#include "Window.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"

class RendererAPI
{
public:
    RendererAPI() = default;
    virtual ~RendererAPI() {}

    virtual void Init() = 0;
    virtual void Shutdown() = 0;
    
    virtual void DrawIndexed(const Shared<VertexArray> vao, u32 count) = 0;
    virtual void DrawLines(const Shared<VertexArray> vao, u32 count) = 0;
    virtual void SetLineWidth(f32 width) = 0;
    virtual void Clear() = 0;
    virtual void SetClearColor(f32 r, f32 g, f32 b, f32 a) = 0;
    virtual void SetViewport(u32 x, u32 y, u32 width, u32 height) = 0;
    virtual void AddWindow(const Shared<Window> window) = 0;
    virtual void RemoveWindow(const Shared<Window> window) = 0;
};
