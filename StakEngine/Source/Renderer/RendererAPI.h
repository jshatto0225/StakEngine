#pragma once

#include "Types.h"
#include "Window.h"

#include <string>

namespace Stak {

class RendererAPI {
public:
  RendererAPI() = default;
  virtual ~RendererAPI() {}

  static Ref<RendererAPI> create(Ref<Window> window, std::string appName);
};

} // namespace Stak
