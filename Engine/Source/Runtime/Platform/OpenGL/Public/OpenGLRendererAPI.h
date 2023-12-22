#pragma once

#include "RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
public:
    OpenGLRendererAPI();
    ~OpenGLRendererAPI();

    void Init() override;
    void Shutdown() override;

    void DrawIndexed(const Shared<VertexArray> vao, u32 count) override;
    void DrawLines(const Shared<VertexArray> vao, u32 count) override;
    void SetLineWidth(f32 width) override;
    void Clear() override;
    void SetClearColor(f32 r, f32 g, f32 b, f32 a) override;
    void SetViewport(u32 x, u32 y, u32 width, u32 height) override;
    void AddWindow(const Shared<Window> window) override;
    void RemoveWindow(const Shared<Window> window) override;

private:
    std::vector<Shared<Window>> mWindows;
};
