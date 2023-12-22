#pragma once

#include <vector>
#include <memory>

#include "Window.h"
#include "ApplicationLayer.h"
#include "Event.h"
#include "Renderer.h"
#include "InputManager.h"

class Application final
{
public:
    static void Init();
    static void Run();
    static void Shutdown();

    static void OnEvent(Event& e);

    template<typename LayerType>
    static void RegisterLayer()
    {
        static_assert(std::is_base_of<ApplicationLayer, LayerType>::value, 
                      "All layers must inherit from ApplicationLayer");
        sApplicationLayers.push_back(new LayerType);
    }

private:
    static bool sRunning;
    // TODO: Remove raw pointer
    static std::vector<ApplicationLayer*> sApplicationLayers;
    //static Shared<Window> sWindow;
    static Unique<InputManager> sInputManager;
};
