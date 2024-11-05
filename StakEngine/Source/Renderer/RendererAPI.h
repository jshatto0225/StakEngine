#pragma once

#include "Types.h"
#include "Window.h"

#include <string>

namespace Stak {

class RendererAPI {
public:
  RendererAPI() = default;
  virtual ~RendererAPI() {}

  virtual void WaitForDevice() = 0;

  static Ref<RendererAPI> Create(Ref<Window> window, std::string appName);
};

} // namespace Stak
