#pragma once

#include "utils/utils.hpp"
#include "input/input.hpp"

class Window;

struct Window_Resize_Event {
    s32 width;
    s32 height;
};

struct Key_Event {
    Input::Key_Code key;
    Input::Input_State state;
};

struct Mouse_Button_Event {
    Input::Mouse_Code button;
    Input::Input_State state;
};

struct Mouse_Move_Event {
    f32 x;
    f32 y;
};
