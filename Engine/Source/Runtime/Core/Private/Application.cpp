#include "Application.h"

#include "Log.h"

bool Application::sRunning;
std::vector<ApplicationLayer*> Application::sApplicationLayers;
Unique<Window> Application::sWindow;
Unique<Renderer> Application::sRenderer;
Unique<InputManager> Application::sInputManager;

void Application::Init()
{
    sWindow = PlatformManager::NewWindow("Stak Engine", 0, 0, 800, 450);
    sRenderer = PlatformManager::NewRenderer();
    sInputManager = PlatformManager::NewInputManager();
    sRunning = true;
}

void Application::Run()
{
    while (sRunning)
    {
        sWindow->Update();
        for (ApplicationLayer* layer : sApplicationLayers)
        {
            layer->Update();
        }
    }
}

void Application::OnEvent(Event& e)
{
    for (ApplicationLayer* layer : sApplicationLayers)
    {
        layer->OnEvent(e);
    }

    switch (e.type)
    {
    case WINDOW_CLOSE:
    {
        WindowCloseEvent* windowCloseEvent = (WindowCloseEvent*)&e;
        if(windowCloseEvent->windowId == sWindow->GetId())
        {
            KillWindowManager();
            sRunning = false;
        }
    } break;
    case WINDOW_RECT_CHANGED:
    {
        WindowRectChangedEvent* windowRectChangedEvent = (WindowRectChangedEvent*)&e;
    } break;
    default:
    {
    } break;
    }
}
