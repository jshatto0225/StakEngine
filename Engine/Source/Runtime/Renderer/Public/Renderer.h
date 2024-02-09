#pragma once

#include "Window.h"

namespace sk {
class Renderer final {
public:
  static void init(const Unique<Window> &window);
  static void shutdown();

private:
  static bool running;
};
} // namespace SK
