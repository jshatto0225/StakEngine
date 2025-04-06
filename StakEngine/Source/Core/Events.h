#pragma once

#include "Types.h"
#include "Input.h"

struct Window_Resize_Event {
    s32 width;
    s32 height;
};

struct Key_Event {
    Key_Code key;
    Input_State state;
};

struct Mouse_Button_Event {
    Mouse_Code button;
    Input_State state;
};

struct Mouse_Move_Event {
    f32 x;
    f32 y;
};

enum class Event_Type {
    WINDOW_RESIZE,
    KEY,
    MOUSE_BUTTON,
    MOUSE_MOVE,
    WINDOW_CLOSE,
};

struct Event {
    Event_Type type;
    union {
        Window_Resize_Event wre;
        Key_Event ke;
        Mouse_Button_Event mbe;
        Mouse_Move_Event mme;
    };
};
