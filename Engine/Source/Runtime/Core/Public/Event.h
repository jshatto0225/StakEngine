#pragma once

#include "Types.h"

enum EventType
{
    WINDOW_RECT_CHANGED,
    WINDOW_CLOSE,
};

struct Event
{
    EventType type;
};

struct WindowCloseEvent : public Event
{
    WindowCloseEvent(u64 windowId)
    {
        Event(WINDOW_CLOSE);
        this->windowId = windowId;
    }

    u64 windowId;
};

struct WindowRectChangedEvent : public Event
{
    WindowRectChangedEvent(u64 windowId, i32 width, i32 height, i32 x, i32 y)
    {
        Event(WINDOW_RECT_CHANGED);
        this->windowId = windowId;
        this->width = width;
        this->height = height;
        this->x = x;
        this->y = y;
    }

    u64 windowId;
    i32 width;
    i32 height;
    i32 x;
    i32 y;
};
