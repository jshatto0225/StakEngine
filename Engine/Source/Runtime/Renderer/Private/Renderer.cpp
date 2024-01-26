#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"

bool Renderer::mRunning;

void Renderer::Init(const Unique<Window>& window)
{
    RenderCommand::Init(window);
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
