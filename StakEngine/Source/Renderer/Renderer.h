#pragma once

#include <imgui.h>

#include "RHI.h"
#include "Window.h"

using RenderFunc = std::function<void(RHIResourceHandle)>;

struct Renderer {
    static const u32 max_frames_in_flight = 2;

    RenderFunc gui_render_func;

    RHIResourceHandle command_list;
    Window *window;
    RHIResourceHandle swapchain_texture;

    u32 offscreen_backbuffer_image_index;
    RHIResourceHandle  offscreen_backbuffers[max_frames_in_flight];
    bool use_offscreen_buffer;

    RHIResourceHandle pipeline_layout;
    RHIResourceHandle pipeline;
};

bool renderer_init(Renderer *renderer, Window *window, bool render_to_offscreen_buffer);

void renderer_shutdown(Renderer *renderer);

bool renderer_init_imgui(Renderer *renderer);

void renderer_imgui_new_frame(Renderer *renderer);

void renderer_shutdown_imgui(Renderer *renderer);

bool renderer_render(Renderer *renderer);

void renderer_add_scene_to_imgui_window(Renderer *renderer);