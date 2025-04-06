#pragma once

#include <StakEngine.h>

bool init(Engine *engine, Application *app);
void shutdown(Engine *engine, Application *app);
void Update(Engine *engine, Application *app);
void on_imgui_render(Engine *engine, Application *app);

struct Editor {
    Input_Callback_Info escape_pressed_callback_info;
    Input_Callback_Info w_pressed_callback_info;
};