#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "IWindow.h"
#include "Events.h"

class Glfw_Window final : public Window {
public:
    Glfw_Window(const Window_Config &cfg);
    ~Glfw_Window() override;

    void update() override;

    void get_size(s32* width, s32* height) override;
    void get_pos(s32* x, s32* y) override;
    
    void init_imgui() override;
    void imgui_new_frame() override;
    void shutdown_imgui() override;
    
    void set_resize_proc(const Resize_Proc &func) override;
    void SetCloseEventFn(const Close_Proc &func) override;
    void set_key_proc(const Key_Proc &func) override;
    void set_mouse_button_proc(const Mouse_Button_Proc &func) override;
    void set_mouse_move_proc(const Mouse_Move_Proc &func) override;
    
public:
    inline GLFWwindow *get_glfw_window() { return handle; }
    
private:
    struct Window_Data {
        Close_Proc close_proc;
        Resize_Proc resize_proc;
        Key_Proc key_proc;
        Mouse_Button_Proc mouse_button_proc;
        Mouse_Move_Proc mouse_move_proc;
        s32 x;
        s32 y;
        s32 width;
        s32 height;
        std::string title;
    };
    
    Window_Data data;
    GLFWwindow *handle = NULL;
    
    static bool glfw_initialized;
};
