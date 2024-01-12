#include "Application.h"

#include "Log.h"
#include "PlatformManager.h"

bool Application::sRunning;
std::vector<ApplicationLayer*> Application::sApplicationLayers;
//Shared<Window> Application::sWindow;
Unique<InputManager> Application::sInputManager;

void Application::Init()
{
    Log::Init();
    Renderer::Init();

    // TODO: No Windows By Default
    //sWindow = PlatformManager::NewWindow("TempWindow", 0, 0, 800, 450);
    //Renderer::AddWindow(sWindow);

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
        //sWindow->Update();
        for(ApplicationLayer* layer : sApplicationLayers)
        {
            layer->Update();
        }
    }
}

void Application::OnEvent(Event& e)
{
    for(ApplicationLayer* layer : sApplicationLayers)
    {
        layer->OnEvent(e);
    }

    switch(e.type)
    {
        case WINDOW_CLOSE:
        {
            WindowCloseEvent* windowCloseEvent = (WindowCloseEvent*)&e;
            //if(windowCloseEvent->windowId == sWindow->GetId() && sRunning)
            //{
                //Shutdown();
            //}
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
        for(ApplicationLayer* layer : sApplicationLayers)
        {
            layer->End();
            delete layer;
        }
        sApplicationLayers.clear();
        Renderer::Shutdown();

        // TODO: No Windows By Default
        //if(sWindow->IsOpen())
        //{
            //sWindow->Close();
        //}

        KillWindowManager();
        Log::Shutdown();
    }
}
