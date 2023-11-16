#pragma once

#include <vector>
#include <memory>

#include "Window.h"
#include "ApplicationLayer.h"
#include "Event.h"

class Application
{
public:
    static void Init();
    static void Run();

    static void HandleEvent(Event& e);

    template<typename LayerType>
    static void RegisterLayer(LayerType* layer)
    {
        static_assert(std::is_base_of<ApplicationLayer, LayerType>(0)::value);
        layers.push_back(layer);
    }

private:
    static bool running;
    static std::vector<ApplicationLayer*> layers;
    static std::unique_ptr<Window> window;
};
