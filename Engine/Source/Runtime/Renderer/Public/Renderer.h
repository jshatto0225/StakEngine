#pragma once

#include "Window.h"

namespace SK
{
    class Renderer final
    {
    public:
        static void Init(const Unique<Window>& window);
        static void Shutdown();

    private:
        static bool mRunning;
    };
}
