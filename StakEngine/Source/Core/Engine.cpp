#include "Engine.h"

#include "Log.h"
#include "RHI.h"
#include "Platform.h"

void destroy_engine(Engine *engine) {
    if (engine->imgui_renderer) {
        destroy_imgui_renderer(engine->imgui_renderer);
    }

    if (engine->input) {
        destroy_input(engine->input);
    }

    if (engine->window) {
        platform_destroy_window(engine->window);
    }

    if (engine->renderer) {
        destroy_renderer(engine->renderer);
    }

    rhi_shutdown();

    platform_shutdown();

    free(engine);
}

Engine *create_engine() {
    auto e = (Engine *) malloc(sizeof(Engine));
    
    if (!e) {
        SK_LOG_ERROR("create_engine malloc failed");
        return nullptr;
    }

    memset(e, 0, sizeof(Engine));

    return e;
}

bool engine_init(Engine *engine, EngineSpecification *spec, Application *app) {
    engine->app = app;

    if (!platform_init()) {
        SK_LOG_ERROR("engine_init platform_init failed");
        return false;
    }

    if (!rhi_init(nullptr, nullptr)) {
        SK_LOG_ERROR("engine_init rhi_init failed");
        return false;
    }

    WindowConfig cfg = {
        spec->window_width,
        spec->window_height,
        spec->window_title,
        ([engine](Event *event) {
            engine_on_event(engine, event);
        })
    };

    engine->window = platform_create_window(&cfg);
    if (!engine->window) {
        SK_LOG_ERROR("engine_init platform_create_window failed");
        return false;
    }

    engine->input = create_input(engine->window);
    if (!engine->input) {
        SK_LOG_ERROR("engine_init create_input failed");
        return false;
    }

    engine->renderer = create_renderer(engine->window);
    if (!engine->renderer) {
        SK_LOG_ERROR("engine_inot create_renderer failed");
        return false;
    }

    engine->imgui_renderer = create_imgui_renderer(engine->renderer);
    if (!engine->imgui_renderer) {
        SK_LOG_ERROR("engine_init create_imgui_renderer failed");
        return false;
    }

    engine->running = true;

    if (engine->app) {
        if (!engine->app->init(engine->app, engine)) {
            SK_LOG_ERROR("engine_init engine->app->init failed");
            return false;
        }
    }

    return true;
}

void engine_run(Engine *engine) {
    while (engine->running) {
        platform_process_messages();

        if (engine->app->update) {
            engine->app->update(engine->app, engine);
        }

        //imgui_renderer_begin_frame(engine->imgui_renderer);
        //if (engine->app->on_imgui_render) {
        //    engine->app->on_imgui_render(engine->app, engine);
        //}
        //imgui_renderer_end_frame(engine->imgui_renderer);

        if (!render(engine->renderer)) {
            SK_LOG_ERROR("engine_run render failed");
        }
    }
}

void engine_on_event(Engine *engine, Event *event) {
    switch (event->type) {
    case EventType::WindowClose:
        engine->running = false;
        break;
    case EventType::Key:
        set_key(engine->input, event->ke.key, event->ke.state);
        break;
    case EventType::MouseButton:
        set_mouse_button(engine->input, event->mbe.button, event->mbe.state);
        break;
    case EventType::MouseMove:
        set_mouse_pos(engine->input, event->mme.x, event->mme.y);
        break;
    default:
        break;
    }

    if (engine->app->on_event) {
        engine->app->on_event(engine->app, engine, event);
    }
}

void engine_close(Engine *engine) {
    engine->running = false;
}