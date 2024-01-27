#include "RenderCommand.h"

#include "RendererAPI.h"
#include "PlatformManager.h"

Shared<RendererAPI> RenderCommand::sRendererAPI;

void RenderCommand::Init(const Unique<Window>& window)
{
    sRendererAPI = PlatformManager::NewRendererAPI(window);
}

void RenderCommand::Shutdown()
{
    sRendererAPI.reset();
}

void RenderCommand::SetViewport(const Vec4u& viewport)
{
    sRendererAPI->SetViewport(viewport);
}

void RenderCommand::SetClearColor(const Vec4f& color)
{
    sRendererAPI->SetClearColor(color);
}

void RenderCommand::Clear()
{
    sRendererAPI->Clear();
}

void RenderCommand::DrawIndexed(Shared<VertexArray> vao, u32 count)
{
    sRendererAPI->DrawIndexed(vao, count);
}

void RenderCommand::DrawLines(Shared<VertexArray> vao, u32 count)
{
    sRendererAPI->DrawLines(vao, count);
}

void RenderCommand::SetLineWidth(f32 width)
{
    sRendererAPI->SetLineWidth(width);
}
