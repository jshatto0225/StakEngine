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

    virtual void Init(const Unique<Window>& window) = 0;
    virtual void Shutdown() = 0;
    
    virtual void DrawIndexed(Shared<VertexArray> vao, u32 count) = 0;
    virtual void DrawLines(Shared<VertexArray> vao, u32 count) = 0;
    virtual void SetLineWidth(f32 width) = 0;
    virtual void Clear() = 0;
    virtual void SetClearColor(const Vec4f& color) = 0;
    virtual void SetViewport(const Vec4u& viewport) = 0;
};
