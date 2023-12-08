#pragma once

#include "Types.h"
#include "Window.h"
#include "InputManager.h"
#include "Renderer.h"

class PlatformManager final
{
public:
    static Unique<Window> NewWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height);
    static Unique<Renderer> NewRenderer();
    static Unique<InputManager> NewInputManager();
};