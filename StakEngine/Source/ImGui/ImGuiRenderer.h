#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "Window.h"
#include "Renderer.h"

struct Imgui_Renderer {
    ImDrawData *draw_data;
    Renderer *renderer;
};

void imgui_renderer_begin_frame(Imgui_Renderer *imgui_renderer);

void imgui_renderer_end_frame(Imgui_Renderer *imgui_renderer);

bool imgui_renderer_init(Imgui_Renderer *imgui_renderer, Renderer *renderer);

void imgui_renderer_shutdown(Imgui_Renderer *imgui_renderer);