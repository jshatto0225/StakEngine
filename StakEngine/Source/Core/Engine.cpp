#include "Engine.h"

#include "Log.h"
#include "Asserts.h"
#include "RHI.h"
#include "Platform.h"

void engine_shutdown(Engine *engine) {
    engine->app->shutdown(engine, engine->app);

    imgui_renderer_shutdown(&engine->imgui_renderer);

    platform_close_window(&engine->window);

    renderer_shutdown(&engine->renderer);

    rhi_shutdown();

    platform_shutdown();
}

bool engine_init(Engine *engine, Engine_Specification *spec, Application *app) {
    engine->app = app;

    engine->name = spec->app_name;

    if (!platform_init()) {
        SK_LOG_ERROR("Failed to initialize platform");
        return false;
    }

    if (!rhi_init(Rhi_Backend::VULKAN)) {
        SK_LOG_ERROR("Failed to initialize RHI");
        return false;
    }

    Window_Config cfg = {
        spec->window_width,
        spec->window_height,
        spec->window_title,
        ([engine](Event *event) {
            engine_on_event(engine, event);
        })
    };

    if (!platform_open_window(&engine->window, &cfg)) {
        SK_LOG_ERROR("Failed to create window");
        return false;
    }

    assert(initialize_input(&engine->input, &engine->window));

    if (!renderer_init(&engine->renderer, &engine->window, spec->render_to_offscreen_buffer)) {
        SK_LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    imgui_renderer_init(&engine->imgui_renderer, &engine->renderer);

    engine->running = true;

    if (engine->app) {
        if (!engine->app->init(engine, engine->app)) {
            SK_LOG_ERROR("Failed to initialize application");
            return false;
        }
    }

    return true;
}

void engine_run(Engine *engine) {
    while (engine->running) {
        platform_process_messages();

        if (engine->app->update) engine->app->update(engine, engine->app);

        imgui_renderer_begin_frame(&engine->imgui_renderer);
        if (engine->app->on_imgui_render) engine->app->on_imgui_render(engine, engine->app);
        imgui_renderer_end_frame(&engine->imgui_renderer);

        if (!renderer_render(&engine->renderer)) {
            SK_LOG_ERROR("Failed to render");
        }
    }
}

void engine_on_event(Engine *engine, Event *event) {
    switch (event->type) {
    case Event_Type::WINDOW_CLOSE:
        engine->running = false;
        break;
    case Event_Type::KEY:
        SetKey(&engine->input, event->ke.key, event->ke.state);
        break;
    case Event_Type::MOUSE_BUTTON:
        SetMouseButton(&engine->input, event->mbe.button, event->mbe.state);
        break;
    case Event_Type::MOUSE_MOVE:
        SetMousePos(&engine->input, event->mme.x, event->mme.y);
        break;
    default:
        break;
    }

    if (engine->app->on_event) engine->app->on_event(engine, event);
}

void engine_close(Engine *engine) {
    engine->running = false;
}