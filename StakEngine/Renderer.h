#pragma once

#include "Window.h"

namespace Renderer
{
void Init(window *Window);
void Shutdown();
void SetViewport(i32 X, i32 Y, i32 Width, i32 Height);
void DrawFrame();
void WaitForDevice();
}
