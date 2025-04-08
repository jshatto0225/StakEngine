#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "Window.h"
#include "Renderer.h"

struct ImguiRenderer {
    ImDrawData *draw_data;
    Renderer *renderer;
};

void imgui_renderer_begin_frame(ImguiRenderer *imgui_renderer);

void imgui_renderer_end_frame(ImguiRenderer *imgui_renderer);

bool imgui_renderer_init(ImguiRenderer *imgui_renderer, Renderer *renderer);

void imgui_renderer_shutdown(ImguiRenderer *imgui_renderer);