#include "Application.h"

#include "Log.h"
#include "PlatformManager.h"

bool Application::sRunning;
std::vector<ApplicationLayer*> Application::sApplicationLayers;
Unique<Window> Application::sWindow;
Unique<Renderer> Application::sRenderer;
Unique<InputManager> Application::sInputManager;

void Application::Init()
{
    Log::Init();
    sWindow = PlatformManager::NewWindow("Stak Engine", 0, 0, 800, 450);
    sRenderer = PlatformManager::NewRenderer();
    sInputManager = PlatformManager::NewInputManager();
    sRunning = true;

    for(ApplicationLayer* layer : sApplicationLayers)
    {
        layer->Start();
    }
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
        if(windowCloseEvent->windowId == sWindow->GetId() && sRunning)
        {
            Shutdown();
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

void Application::Shutdown()
{
    if(sRunning)
    {
        SK_CORE_WARN("Application Shutdown");
        sRunning = false;
        for (ApplicationLayer* layer : sApplicationLayers)
        {
            layer->End();
            delete layer;
        }
        sApplicationLayers.clear();
        if(sRenderer->IsRunning())
        {
            sRenderer->Shutdown();
        }
        if(sWindow->IsOpen())
        {
            sWindow->Close();
        }
        KillWindowManager();
        Log::Shutdown();
    }
}
