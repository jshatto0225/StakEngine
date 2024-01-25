#pragma once

#include <array>

#include "Types.h"
#include "Window.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Vec4Types.h"

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
    virtual void SetClearColor(const Vec4f& color) = 0;
    virtual void SetViewport(const Vec4u& viewport) = 0;
    virtual void AddWindow(u64 window) = 0;
    virtual void RemoveWindow(u64 window) = 0;
    virtual void MakeContextCurrent(u64 window) = 0;
};
