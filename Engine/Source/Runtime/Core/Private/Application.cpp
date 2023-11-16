#include "Application.h"
#include "Input.h"
#include "log.h"

bool Application::running;
std::vector<ApplicationLayer*> Application::layers;
std::unique_ptr<Window> Application::window;

void Application::Init()
{
    window = MakeWindow("Stak Engine", 0, 0, 1280, 720);
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

void Application::HandleEvent(Event& e)
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
