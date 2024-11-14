#pragma once

#include "IWindow.h"

#include <imgui.h>

class Renderer {
public:
  Renderer(TRef<IWindow> window);
  ~Renderer();
};
