#pragma once

#include "utils.hpp"
#include "input.hpp"

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
