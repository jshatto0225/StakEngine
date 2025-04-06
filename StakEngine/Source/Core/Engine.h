#pragma once

#include <vector>
#include <string>

#include "Events.h"
#include "Renderer.h"
#include "Input.h"
#include "ImGuiRenderer.h"

struct Engine;

struct Application {
    bool (*init)            (Engine *, Application *);
    void (*shutdown)        (Engine *, Application *);
    void (*on_imgui_render) (Engine *, Application *);
    void (*update)          (Engine *, Application *);
    void (*on_event)        (Engine *, Event *);

    void *user_data;
};

struct Engine_Specification {
    const char *window_title;
    const char *app_name;
    s32 window_width;
    s32 window_height;
    bool render_to_offscreen_buffer;
};

struct Engine {
    Application *app;
    Window window;
    Renderer renderer;
    Input input;
    bool running;
    Imgui_Renderer imgui_renderer;
    const char *name;
};

bool engine_init(Engine *engine, Engine_Specification *spec, Application *app_impl);
void engine_shutdown(Engine *engine);

void engine_run(Engine *engine);
void engine_close(Engine *engine);

void engine_on_event(Engine *engine, Event *event);