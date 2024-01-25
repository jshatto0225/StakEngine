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

void RenderCommand::AddWindow(u64 window)
{
    sRendererAPI->AddWindow(window);
}

void RenderCommand::RemoveWindow(u64 window)
{
    sRendererAPI->RemoveWindow(window);
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

void RenderCommand::MakeContextCurrent(u64 window)
{
    sRendererAPI->MakeContextCurrent(window);
}
