#pragma once

#include "../Core/Window.h"

namespace Stak {
class Renderer {
public:
  virtual void SetViewport(i32 x, i32 y, i32 width, i32 height) = 0;
  virtual void DrawFrame() = 0;
  virtual void WaitForGpu() = 0;

  static Scope<Renderer> Create(Ref<Window> window);
};
} // namespace Stak
