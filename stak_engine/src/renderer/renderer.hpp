#pragma once

#include <imgui.h>

#include "platform/platform.hpp"
#include "rhi/rhi.hpp"

namespace Renderer {

void init(Platform::Window* window, Rhi::Vendor vendor, const std::string &app_name);
void shutdown();
void update();

}
