#pragma once

#include <imgui.h>

#include "platform/platform.hpp"
#include "rhi/rhi.hpp"
#include "utils/utils.hpp"

namespace Renderer {

void init(Platform::Window* window, Rhi::Vendor vendor, const String &app_name);
void shutdown();
void update();

}
