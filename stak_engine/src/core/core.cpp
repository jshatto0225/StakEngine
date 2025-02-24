#include "core.hpp"

#include "gui/gui.hpp"
#include "platform/platform.hpp"
#include "log/log.hpp"

// Private interface
static bool running = true;

static void window_resized(Window_Resize_Event *event) {

}

static void key_event(Key_Event *event) {
    Input::set_key(event->key, event->state);
}

static void mouse_button_event(Mouse_Button_Event *event) {
    Input::set_mouse_button(event->button, event->state);
}

static void mouse_move_event(Mouse_Move_Event *event) {
    Input::set_mouse_pos(event->x, event->y);
}

static void null_init() {}
static void null_update() {}
static void null_shutdown() {}
static void null_gui_render() {}

// Public interface
void Core::run(App *app) {
    if (app == nullptr) {
        return;
    }

    Log::init();

    if (app->init == nullptr) {
        app->init = null_init;
    }
    if (app->update == nullptr) {
        app->update = null_update;
    }
    if (app->shutdown == nullptr) {
        app->shutdown = null_shutdown;
    }
    if (app->gui_render == nullptr) {
        app->gui_render = null_gui_render;
    }

    Platform::Window_Callbacks callbacks = {
        close,
        window_resized,
        key_event,
        mouse_button_event,
        mouse_move_event,
    };
    
    Platform::Window *window = Platform::create_window(&app->window_config, &callbacks);
    ASSERT(window);

    Renderer::init(window, app->renderer_vendor, app->name);

    Gui::init(window);

    app->init();
    
    while (running) {
        Platform::poll_events();
        
        app->update();

        Gui::begin_frame();
        {
            app->gui_render();
        }
        Gui::end_frame();

        Renderer::update();
    }

    app->shutdown();
    Gui::shutdown();
    Renderer::shutdown();
    Platform::destroy_window(&window);
}

void Core::close() {
    running = false;
}
