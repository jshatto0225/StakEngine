#pragma once

#include <StakEngine.h>

bool init(Application *app, Engine *engine);
void update(Application *app, Engine *engine);
void on_imgui_render(Application *app, Engine *engine);
EngineSpecification get_engine_spec(Application *app);

struct Editor {
    InputCallbackInfo escape_pressed_callback_info;
    InputCallbackInfo w_pressed_callback_info;
};