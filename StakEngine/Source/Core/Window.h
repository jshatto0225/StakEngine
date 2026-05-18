#pragma once

#include <functional>

#include "Events.h"
#include "Types.h"

using EventFunction = std::function<void(Event *)>;

struct WindowConfig {
    s32 width;
    s32 height;
    const char *title;
    EventFunction event_function;
};

typedef Handle Window;
