#pragma once

#include "RendererAPI.h"
#include "VertexArray.h"
#include "Window.h"

class RenderCommand final
{
public:
    static void Init();
    static void Shutdown();
    static void AddWindow(const Shared<Window> window);
    static void RemoveWindow(const Shared<Window> window);
    static void SetViewport(u32 x, u32 y, u32 width, u32 height);
    static void SetClearColor(f32 r, f32 g, f32 b, f32 a);
    static void Clear();
    static void DrawIndexed(const Shared<VertexArray> vao, u32 count);
    static void DrawLines(const Shared<VertexArray> vao, u32 count);
    static void SetLineWidth(f32 width);

private:
    static Shared<RendererAPI> sRendererAPI;
};