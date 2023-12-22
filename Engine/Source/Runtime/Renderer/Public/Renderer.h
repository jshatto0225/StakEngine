#pragma once

#include "Window.h"

class Renderer final
{
public:
    static void Init();
    static void Shutdown();
    static void AddWindow(Shared<Window> window);
    static void RemoveWindow(Shared<Window> window);

private:
    static bool mRunning;
};
