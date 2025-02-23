#pragma once

#include "Types.h"
#include "FInput.h"

class Window;

struct Window_Resize_Event {
    const s32 WIDTH;
    const s32 HEIGHT;
};

struct Key_Event {
    const Key_Code Key;
    const Input_State State;
};

struct Mouse_Button_Event {
    const Mouse_Code Button;
    const Input_State State;
};

struct Mouse_Move_Event {
    const f32 X;
    const f32 Y;
};
