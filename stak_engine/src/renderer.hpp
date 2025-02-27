#pragma once

#include "platform.hpp"
#include "rhi.hpp"
#include "utils.hpp"
#include "window.hpp"

void init_renderer(Ref<Window> window, Rhi_Vendor vendor, const String &app_name);
void shutdown_renderer();
void update_renderer();

