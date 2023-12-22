#include "RenderCommand.h"

#include "RendererAPI.h"
#include "PlatformManager.h"

Shared<RendererAPI> RenderCommand::sRendererAPI;

void RenderCommand::Init()
{
    sRendererAPI = PlatformManager::NewRendererAPI();
    sRendererAPI->Init();
}

void RenderCommand::Shutdown()
{
    sRendererAPI->Shutdown();
}

void RenderCommand::AddWindow(const Shared<Window> window)
{
    sRendererAPI->AddWindow(window);
}

void RenderCommand::RemoveWindow(const Shared<Window> window)
{
    sRendererAPI->RemoveWindow(window);
}

void RenderCommand::SetViewport(u32 x, u32 y, u32 width, u32 height)
{
    sRendererAPI->SetViewport(x, y, width, height);
}

void RenderCommand::SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
    sRendererAPI->SetClearColor(r, g, b, a);
}

void RenderCommand::Clear()
{
    sRendererAPI->Clear();
}

void RenderCommand::DrawIndexed(const Shared<VertexArray> vao, u32 count)
{
    sRendererAPI->DrawIndexed(vao, count);
}

void RenderCommand::DrawLines(const Shared<VertexArray> vao, u32 count)
{
    sRendererAPI->DrawLines(vao, count);
}

void RenderCommand::SetLineWidth(f32 width)
{
    sRendererAPI->SetLineWidth(width);
}
