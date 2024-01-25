#include "Application.h"

#include "Log.h"
#include "PlatformManager.h"
#include "WindowManager.h"

bool Application::sRunning;
std::vector<ApplicationLayer*> Application::sApplicationLayers;
Unique<InputManager> Application::sInputManager;

void Application::Init()
{
    WindowManager::Init();
    Renderer::Init();
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
        for(ApplicationLayer* layer : sApplicationLayers)
        {
            layer->Update();
        }
        WindowManager::Update();
    }
}

void Application::OnEvent(Event& e)
{
    if(sRunning)
    {
        for(ApplicationLayer* layer : sApplicationLayers)
        {
            layer->OnEvent(e);
            // TODO: wierd way to prevent function from continuing after shutdown
            // Implement shutdown request or something
            // Application::RequestShutdown() { mRunning = false; }
            // Shut down on next loop
            // Or just handle logging in the entry point (Doing this for now)
            if(!sRunning)
            {
                return;
            }
        }

        switch(e.type)
        {
            case WINDOW_CLOSE:
            {
                WindowCloseEvent* wce = (WindowCloseEvent*)&e;
                WindowManager::CloseWindow(wce->windowId);
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
        WindowManager::Shutdown();
    }
}
