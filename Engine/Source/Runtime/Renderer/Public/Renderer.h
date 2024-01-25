#pragma once

#include "Window.h"

class Renderer final
{
public:
    static void Init();
    static void Shutdown();
    static void AddWindow(u64 window);
    static void RemoveWindow(u64 window);
    static void MakeContextCurrent(u64 window);

private:
    static bool mRunning;
};
