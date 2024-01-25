#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"

bool Renderer::mRunning;

void Renderer::Init()
{
    RenderCommand::Init();
    mRunning = true;
}

void Renderer::Shutdown()
{
    if(mRunning)
    {
        mRunning = false;
        Renderer2D::Shutdown();
        RenderCommand::Shutdown();
    }
}

void Renderer::AddWindow(u64 window)
{
    RenderCommand::AddWindow(window);
}

void Renderer::RemoveWindow(u64 window)
{
    RenderCommand::RemoveWindow(window);
}

void Renderer::MakeContextCurrent(u64 window)
{
    RenderCommand::MakeContextCurrent(window);
    Renderer2D::Init();
}
