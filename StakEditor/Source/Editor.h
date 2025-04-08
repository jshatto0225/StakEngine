#pragma once

#include <StakEngine.h>

bool init(Engine *engine, Application *app);
void shutdown(Engine *engine, Application *app);
void update(Engine *engine, Application *app);
void on_imgui_render(Engine *engine, Application *app);

struct Editor {
    InputCallbackInfo escape_pressed_callback_info;
    InputCallbackInfo w_pressed_callback_info;
};