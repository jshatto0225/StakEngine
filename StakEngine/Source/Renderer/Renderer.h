#pragma once

#include <imgui.h>

#include "RHI.h"
#include "Window.h"

struct Renderer {
    static const u32 max_frames_in_flight = 2;

    Window *window;

    u32 offscreen_backbuffer_image_index;
    bool use_offscreen_buffer;

    RHIDevice device;
    RHIQueue queue;
    RHIPipeline pipeline;
    RHISemaphore semaphore;

    u64 next_frame;
};

bool renderer_init(Renderer *renderer, Window *window, bool render_to_offscreen_buffer);

void renderer_shutdown(Renderer *renderer);

bool renderer_render(Renderer *renderer);