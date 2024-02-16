#pragma once

#include "Window.h"

namespace sk {
void renderer_init(const Unique<Window> &window);
void renderer_shutdown();
} // namespace sk
