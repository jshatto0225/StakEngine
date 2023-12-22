#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"

bool Renderer::mRunning;

void Renderer::Init()
{
    RenderCommand::Init();
    Renderer2D::Init();
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

void Renderer::AddWindow(Shared<Window> window)
{
    RenderCommand::AddWindow(window);
}

void Renderer::RemoveWindow(Shared<Window> window)
{
    RenderCommand::RemoveWindow(window);
}
