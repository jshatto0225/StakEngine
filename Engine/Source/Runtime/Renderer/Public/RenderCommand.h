#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Window.h"

namespace SK
{
    class RenderCommand final
    {
    public:
        static void Init(const Unique<Window>& window);
        static void Shutdown();
        static void SetViewport(const Vec4u& viewport);
        static void SetClearColor(const Vec4f& color);
        static void Clear();
        static void DrawIndexed(Shared<VertexArray> vao, u32 count);
        static void DrawLines(Shared<VertexArray> vao, u32 count);
        static void SetLineWidth(f32 width);

    private:
        static Shared<RendererAPI> sRendererAPI;
    };
}

