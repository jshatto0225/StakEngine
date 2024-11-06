#pragma once

#include "Window.h"
#include "RendererAPI.h"

namespace Stak {
class Renderer {
public:
  static void init(Ref<Window> window);
  static void shutdown();

  static inline Ref<RendererAPI> &getAPI() { return s_API; }

private:
  static Ref<RendererAPI> s_API;
};
} // namespace Stak
