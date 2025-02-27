#pragma once

#include <imgui.h>

#include "platform.hpp"
#include "rhi.hpp"

void init_imgui(Ref<Window> window);
void shutdown_imgui();
void begin_imgui_frame();
void end_imgui_frame();
void render_imgui(Ref<Rhi_Command_List> command_list);
