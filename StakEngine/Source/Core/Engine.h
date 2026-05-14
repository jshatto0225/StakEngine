#pragma once

#include "Events.h"
#include "Renderer.h"
#include "Input.h"
#include "ImGuiRenderer.h"

struct Engine;

struct EngineSpecification {
    const char *window_title;
    s32 window_width;
    s32 window_height;
};

struct Application {
    bool (*init)            (Application *app, Engine *engine);
    void (*on_imgui_render) (Application *app, Engine *engine);
    void (*update)          (Application *app, Engine *engine);
    void (*on_event)        (Application *app, Engine *engine, Event *event);

    EngineSpecification (*get_engine_spec)(Application *app);

    void *user_data;
};

struct Engine {
    Application *app;
    Window window;
    Renderer *renderer;
    Input *input;
    bool running;
    ImguiRenderer *imgui_renderer;
};

Engine *create_engine();

bool engine_init(Engine *engine, EngineSpecification *spec, Application *app_impl);
void destroy_engine(Engine *engine);

void engine_run(Engine *engine);
void engine_close(Engine *engine);
void engine_on_event(Engine *engine, Event *event);