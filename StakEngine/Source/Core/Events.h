#pragma once

#include "Types.h"
#include "Input.h"

struct FWindowResizeEvent {
    FSInt32 Width;
    FSInt32 Height;
};

struct FKeyEvent {
    EKeyCode Key;
    EInputState State;
};

struct FMouseButtonEvent {
    EMouseCode Button;
    EInputState State;
};

struct FMouseMoveEvent {
    FFloat X;
    FFloat Y;
};

enum class EEventType {
    WINDOW_RESIZE,
    KEY,
    MOUSE_BUTTON,
    MOUSE_MOVE,
    WINDOW_CLOSE,
};

struct FEvent {
    EEventType Type;
    union {
        FWindowResizeEvent WRE;
        FKeyEvent KE;
        FMouseButtonEvent MBE;
        FMouseMoveEvent MME;
    };
};
