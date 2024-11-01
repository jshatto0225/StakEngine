#pragma once

#include "Window.h"
#include "RendererAPI.h"

namespace Stak {
class Renderer {
public:
  Renderer(Ref<Window> window);
  ~Renderer();

private:
  Scope<RendererAPI> m_API;
};
} // namespace Stak
