#pragma once

#include <imgui.h>

#include "RHI.h"
#include "Window.h"

struct Renderer {
    static const u32 max_frames_in_flight = 2;

    Window window;

    RHIDevice device;
    RHIQueue queue;
    RHIPipeline pipeline;
    RHISemaphore semaphore;

    u64 next_frame;
};

Renderer *create_renderer(Window window);

void destroy_renderer(Renderer *renderer);

bool render(Renderer *renderer);