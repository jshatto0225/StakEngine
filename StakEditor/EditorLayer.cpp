#include "EditorLayer.h"

namespace EditorLayer
{

void
Init()
{
}

void
Shutdown()
{
}

void
Update()
{
    if (Input::KeyDown(Input::K_ESCAPE))
    {
        Log::Trace("Escape pressed. Quitting.");
        Application::RequestShutdown();
    }
}

void
OnEvent(const event *Event)
{
}

}
