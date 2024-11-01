#pragma once

#include "Types.h"
#include "Window.h"

namespace Stak {

class RendererAPI {
public:
  RendererAPI() = default;
  virtual ~RendererAPI() {}

  virtual void WaitForDevice() = 0;

  static Scope<RendererAPI> Create(Ref<Window> window);
};

} // namespace Stak