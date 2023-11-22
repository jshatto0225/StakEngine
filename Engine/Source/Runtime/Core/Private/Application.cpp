#include "Application.h"
#include "Input.h"
#include "Log.h"

bool Application::sRunning;
std::vector<ApplicationLayer*> Application::sApplicationLayers;
Unique<Window> Application::sWindow;
Unique<Renderer> Application::sRenderer;

void Application::Init()
{
    sWindow = MakeWindow("Stak Engine", 0, 0, 800, 450);
    sRenderer = MakeRenderer();
    sRunning = true;
}

void Application::Run()
{
    while (sRunning)
    {
        sWindow->Update();
        for (const auto& layer : sApplicationLayers)
        {
            layer->Update();
        }
    }
}

void Application::OnEvent(Event& e)
{
    switch (e.type)
    {
    case WINDOW_CLOSE:
    {
        // TODO: Make window manager class
        if (e.data == sWindow->GetId())
        {
            sRunning = false;
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
