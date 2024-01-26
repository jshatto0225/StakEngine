#include "Application.h"

#include "Log.h"
#include "PlatformManager.h"

bool Application::sRunning;
std::vector<ApplicationLayer*> Application::sApplicationLayers;
Unique<InputManager> Application::sInputManager;
Unique<Window> Application::sWindow;

void Application::Init()
{
    sWindow = PlatformManager::NewWindow("Stak Applciation", 100, 100, 1280, 720);
    Renderer::Init(sWindow);
    sInputManager = PlatformManager::NewInputManager();
    sRunning = true;

    for(ApplicationLayer* layer : sApplicationLayers)
    {
        layer->Start();
    }
}

void Application::Run()
{
    while(sRunning)
    {
        sWindow->Update();

        for(ApplicationLayer* layer : sApplicationLayers)
        {
            layer->Update();
        }
    }
}

void Application::OnEvent(Event& e)
{
    if(sRunning)
    {
        for(ApplicationLayer* layer : sApplicationLayers)
        {
            layer->OnEvent(e);
        }

        switch(e.type)
        {
            case WINDOW_CLOSE:
            {
                WindowCloseEvent* wce = (WindowCloseEvent*)&e;
                if(wce->windowId == sWindow->GetId())
                {
                    Application::Shutdown();
                }
            } break;
        }
    }
}

void Application::Shutdown()
{
    if(sRunning)
    {
        SK_CORE_WARN("Application Shutdown");
        sRunning = false;
        for(ApplicationLayer* layer : sApplicationLayers)
        {
            layer->End();
            delete layer;
        }
        sApplicationLayers.clear();
        Renderer::Shutdown();
        sWindow.reset();
    }
}
