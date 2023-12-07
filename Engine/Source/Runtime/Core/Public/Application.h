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

    static void OnEvent(Event& e);

    template<typename LayerType>
    static void RegisterLayer(LayerType* layer)
    {
        static_assert(std::is_base_of<ApplicationLayer, LayerType>::value, 
                      "All layers must inherit from ApplicationLayer");
        sApplicationLayers.push_back(layer);
    }

private:
    static bool sRunning;
    // TODO: Remove raw pointer
    static std::vector<ApplicationLayer*> sApplicationLayers;
    static Unique<Window> sWindow;
    static Unique<Renderer> sRenderer;
    static Unique<InputManager> sInputManager;
};
