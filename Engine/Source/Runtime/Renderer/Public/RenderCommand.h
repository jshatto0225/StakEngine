#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Window.h"

class RenderCommand final
{
public:
    static void Init();
    static void Shutdown();
    static void AddWindow(u64 window);
    static void RemoveWindow(u64 window);
    static void SetViewport(const Vec4u& viewport);
    static void SetClearColor(const Vec4f& color);
    static void Clear();
    static void DrawIndexed(const Shared<VertexArray> vao, u32 count);
    static void DrawLines(const Shared<VertexArray> vao, u32 count);
    static void SetLineWidth(f32 width);
    static void MakeContextCurrent(u64 window);

private:
    static Shared<RendererAPI> sRendererAPI;
};