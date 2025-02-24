#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "platform/platform.hpp"

namespace Gui {

void init(Platform::Window *window);
void shutdown();
void begin_frame();
void end_frame();

}
