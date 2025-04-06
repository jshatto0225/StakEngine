#pragma once

#include <imgui.h>

#include "RHI.h"
#include "Window.h"

using Render_Func = std::function<void(Rhi_Resource_Handle)>;

struct Renderer {
    static const u32 max_frames_in_flight = 2;

    Render_Func gui_render_func;

    Rhi_Resource_Handle command_list;
    Window *window;
    Rhi_Resource_Handle swapchain_texture;

    u32 offscreen_backbuffer_image_index;
    Rhi_Resource_Handle  offscreen_backbuffers[max_frames_in_flight];
    bool use_offscreen_buffer;

    Rhi_Resource_Handle pipeline_layout;
    Rhi_Resource_Handle pipeline;
};

bool renderer_init(Renderer *renderer, Window *window, bool render_to_offscreen_buffer);

void renderer_shutdown(Renderer *renderer);

bool renderer_init_imgui(Renderer *renderer);

void renderer_imgui_new_frame(Renderer *renderer);

void renderer_shutdown_imgui(Renderer *renderer);

bool renderer_render(Renderer *renderer);

void renderer_add_scene_to_imgui_window(Renderer *renderer);