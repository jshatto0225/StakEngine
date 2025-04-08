#pragma once

#include <functional>

#include "Events.h"
#include "Types.h"
#include "RHI.h"

using EventFunction = std::function<void(Event *)>;

struct WindowConfig {
    s32 width;
    s32 height;
    const char *title;
    EventFunction event_function;
};

struct Window {
    s32 x;
    s32 y;
    s32 width;
    s32 height;
    s32 framebuffer_width;
    s32 framebuffer_height;
    const char *title;
    EventFunction event_function;
    bool open;

    // NOTE: Only platform functions should touch this
    //   because it is probably implemented as a pointer/handle
    //   to a platform window
    Handle platform_handle;

    RHIResourceHandle swapchain;
};
