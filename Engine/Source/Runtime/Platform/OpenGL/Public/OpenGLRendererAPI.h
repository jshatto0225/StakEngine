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

    void Init(const Unique<Window>& window) override;
    void Shutdown() override;

    void DrawIndexed(Shared<VertexArray> vao, u32 count) override;
    void DrawLines(Shared<VertexArray> vao, u32 count) override;
    void SetLineWidth(f32 width) override;
    void Clear() override;
    void SetClearColor(const Vec4f& color) override;
    void SetViewport(const Vec4u& viewport) override;

private:
    Unique<GLContext> mContext;
};
