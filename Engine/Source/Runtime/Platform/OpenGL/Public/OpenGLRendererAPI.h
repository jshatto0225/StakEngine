#pragma once

#include <unordered_map>

#include "RendererAPI.h"
#include "Types.h"
#include "GlContext.h"

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
    void SetClearColor(const Vec4f& color) override;
    void SetViewport(const Vec4u& viewport) override;

    // Decide if windows should be in seperate contexts
    // So far yes
    void AddWindow(u64 window) override;
    void RemoveWindow(u64 window) override;
    void MakeContextCurrent(u64 window) override;

private:
    std::unordered_map<u64, Unique<GLContext>> mIdContextMap;
};
