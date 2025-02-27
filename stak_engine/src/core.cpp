#include "core.hpp"

#include "gui.hpp"
#include "platform.hpp"
#include "log.hpp"
#include "window.hpp"

// Private interface
static bool running = true;

static void window_resized(Window_Resize_Event &event) {

}

static void key_event(Key_Event &event) {
    set_key(event.key, event.state);
}

static void mouse_button_event(Mouse_Button_Event &event) {
    set_mouse_button(event.button, event.state);
}

static void mouse_move_event(Mouse_Move_Event &event) {
    set_mouse_pos(event.x, event.y);
}

static void null_init() {}
static void null_update() {}
static void null_shutdown() {}
static void null_imgui_render() {}

// Public interface
void run_app(App &app) {
    init_log();

    if (app.init == nullptr) {
        app.init = null_init;
    }
    if (app.update == nullptr) {
        app.update = null_update;
    }
    if (app.shutdown == nullptr) {
        app.shutdown = null_shutdown;
    }
    if (app.imgui_render == nullptr) {
        app.imgui_render = null_imgui_render;
    }

    Window_Callbacks callbacks = {
        close_app,
        window_resized,
        key_event,
        mouse_button_event,
        mouse_move_event,
    };
    
    auto window = create_window(app.window_config, callbacks);
    ASSERT(window != nullptr);

    init_renderer(window, app.renderer_vendor, app.name);

    app.init();
    
    while (running) {
        poll_events();
        
        app.update();

        begin_imgui_frame();
        {
            app.imgui_render();
        }
        end_imgui_frame();

        update_renderer();
    }

    app.shutdown();
    
    shutdown_renderer();
}

void close_app() {
    running = false;
}
