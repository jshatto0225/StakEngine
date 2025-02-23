#include "FApplication.h"

#include "FLog.h"
#include "FImGuiLayer.h"
#include "Asserts.h"
#include "RHI.h"

void App::add_layer(App_Layer *Layer) {
    layer_stack.push(Layer);
}

App::~App() {
    layer_stack.clear();

    renderer = nullptr;
    Rhi::shutdown();
    input = nullptr;
    window = nullptr;

    instance = nullptr;
}

App::App(const App_Spec &spec) {
    ASSERT(!instance);

    instance = this;

    Window_Config cfg = {
        spec.window_width,
        spec.window_height,
        spec.window_title
    };
    window = Window::create(cfg);
    window->set_resize_proc([this](const Window_Resize_Event &event) { 
        this->on_window_resize(event);
    });
    window->SetCloseEventFn([this]() { 
        this->on_window_close(); 
    });
    window->set_key_proc([this](const Key_Event & event) {
        this->on_key_event(event);
    });
    window->set_mouse_button_proc([this](const Mouse_Button_Event & event) {
        this->on_mouse_button_event(event);
    });
    window->set_mouse_move_proc([this](const Mouse_Move_Event & event) {
        this->on_mouse_move_event(event);
    });

    input = create_ref<Input>();

    Rhi::init();

    renderer = create_ref<Renderer>();

    imgui_layer = new Imui_Layer();

    add_layer(imgui_layer);

    running = true;
}

void App::run() {
    while (running) {
        for (App_Layer *layer : layer_stack) {
            layer->update();
        }

        imgui_layer->begin_frame();
        {
            for (App_Layer * layer : layer_stack) {
                layer->on_imgui_render();
            }
        }
        imgui_layer->end_frame();
        
        renderer->render();
        
        window->update();
    }
}

void App::on_window_resize(const Window_Resize_Event &event) {
    Rhi::get().is_framebuffer_resized();
    for (App_Layer * layer : layer_stack) {
        layer->on_window_resize(event);
    }
}

void App::on_window_close() {
    close();
}

void App::on_key_event(const Key_Event & event) {
    input->set_key(event.Key, event.State);
}

void App::on_mouse_button_event(const Mouse_Button_Event & event) {
    input->set_mouse_button(event.Button, event.State);
}

void App::on_mouse_move_event(const Mouse_Move_Event & event) {
    input->set_mouse_pos(event.X, event.Y);
}

void App::close() {
    running = false;
}
