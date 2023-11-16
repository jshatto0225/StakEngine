#pragma once

#include "Types.h"

enum EventType
{
    WINDOW_RECT_CHANGED,
    WINDOW_CLOSE
};

struct Event
{
    EventType type;
    u64 size;
    u64 data;
};
