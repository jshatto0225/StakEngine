#include "Application.h"
#include "Input.h"
#include "Log.h"

bool Application::running;
std::vector<ApplicationLayer*> Application::layers;
Scope<Window> Application::window;

void Application::Init()
{
    window = MakeWindow("Stak Engine", 0, 0, 800, 450);
    running = true;
}

void Application::Run()
{
    while (running)
    {
        window->Update();
        for (const auto& layer : layers)
        {
            layer->Update();
        }

        SK_CORE_TRACE("(%d, %d)", Input::GetMouseX(), Input::GetMouseY());
    }
}

void Application::OnEvent(Event& e)
{
    switch (e.type)
    {
    case WINDOW_CLOSE:
    {
        if (e.data == window->GetId())
        {
            running = false;
            KillWindowManager();
        }
    } break;
    case WINDOW_RECT_CHANGED:
    {

    } break;
    default:
    {

    } break;
    }
}
