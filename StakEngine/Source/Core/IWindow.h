#pragma once

#include <functional>

#include "Events.h"
#include "Types.h"

struct Window_Config {
    s32 width;
    s32 height;
    const char *title;
};

class Window {
public:

    Window() = default;
    virtual ~Window() {}

    virtual void update() = 0;

    virtual void get_size(s32 *width, s32 *height) = 0;
    virtual void get_pos(s32* x, s32* y) = 0;

    using Resize_Proc       = std::function<void(const Window_Resize_Event &)>;
    using Key_Proc          = std::function<void(const Key_Event &)>;
    using Mouse_Button_Proc = std::function<void(const Mouse_Button_Event &)>;
    using Mouse_Move_Proc   = std::function<void(const Mouse_Move_Event &)>;

    virtual void set_resize_proc(const Resize_Proc &func) = 0;
    virtual void set_key_proc(const Key_Proc & func) = 0;
    virtual void set_mouse_button_proc(const Mouse_Button_Proc & func) = 0;
    virtual void set_mouse_move_proc(const Mouse_Move_Proc & func) = 0;

    using Close_Proc = std::function<void()>;
    virtual void SetCloseEventFn(const Close_Proc & func) = 0;

    virtual void init_imgui() = 0;
    virtual void imgui_new_frame() = 0;
    virtual void shutdown_imgui() = 0;

    static Scope<Window> create(const Window_Config &Cfg);
};
