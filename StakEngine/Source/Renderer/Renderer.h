#pragma once

#include "Window.h"
#include "RendererAPI.h"

namespace Stak {
class Renderer {
public:
  static void Init(Ref<Window> window);
  static void Shutdown();

  static inline Ref<RendererAPI> &GetAPI() { return s_API; }

private:
  static Ref<RendererAPI> s_API;
};
} // namespace Stak
