#pragma once

#include <vector>

#include "Window.h"
#include "ApplicationLayer.h"
#include "Event.h"
#include "InputManager.h"

namespace SK
{
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
        static Unique<InputManager> sInputManager;
        static Unique<Window> sWindow;
    };
}

