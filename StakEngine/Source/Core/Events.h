#pragma once

#include "Types.h"
#include "InputEnums.h"

struct WindowResizeEvent {
    s32 width;
    s32 height;
};

struct KeyEvent {
    KeyCode key;
    InputState state;
};

struct MouseButtonEvent {
    MouseCode button;
    InputState state;
};

struct MouseMoveEvent {
    f32 x;
    f32 y;
};

enum class EventType {
    WindowResize,
    Key,
    MouseButton,
    MouseMove,
    WindowClose,
};

struct Event {
    EventType type;
    union {
        WindowResizeEvent wre;
        KeyEvent ke;
        MouseButtonEvent mbe;
        MouseMoveEvent mme;
    };
};
