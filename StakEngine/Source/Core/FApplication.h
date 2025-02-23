#pragma once

#include <vector>
#include <string>

#include "IApplicationLayer.h"
#include "Events.h"
#include "FRenderer.h"
#include "FInput.h"
#include "FImGuiLayer.h"

struct App_Spec {
    const char *window_title;
    s32 window_width;
    s32 window_height;
};

class App final {
public:
    App(const App_Spec &Spec);
    ~App();

    static App &get() { ASSERT(instance);  return *instance; }

    void run();
    void add_layer(App_Layer *layer);
    void close();
  
    inline const char *get_name() const { return name.c_str(); }
    inline const Ref<Window> get_window() const { return window; }

private:
    void on_window_close();
    void on_window_resize(const Window_Resize_Event &event);
    void on_key_event(const Key_Event &event);
    void on_mouse_button_event(const Mouse_Button_Event &event);
    void on_mouse_move_event(const Mouse_Move_Event &event);

private:
    inline static App *instance = nullptr;

private:
    Ref<Window> window;
    Ref<Renderer> renderer;
    Ref<Input> input;
    Layer_Stack layer_stack;
    bool running;
    Imui_Layer *imgui_layer;
    std::string name = "Stak Application";
};
