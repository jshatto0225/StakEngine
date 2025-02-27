#pragma once

#include "utils.hpp"
#include "events.hpp"

struct Window_Config {
	String title = "";

	s32 width = 0;
	s32 height = 0;
	s32 x = 0;
	s32 y = 0;
};

struct Window_Callbacks {
	void (*close)() = nullptr;
	void (*resize)(Window_Resize_Event &) = nullptr;
	void (*key)(Key_Event &) = nullptr;
	void (*mouse_button)(Mouse_Button_Event &) = nullptr;
	void (*mouse_move)(Mouse_Move_Event &) = nullptr;
};

struct Rhi_Device;

struct Window {
    virtual ~Window() {}
    
    virtual void enable_raw_input() = 0;
    virtual void disable_raw_input() = 0;
    
    virtual void hide_cursor() = 0;
    virtual void show_cursor() = 0;
    
    virtual void get_size(s32 *width, s32 *height) = 0;
    
    virtual void get_vulkan_surface(void *instance, void *surface) = 0;

    virtual Ref<Rhi_Device> get_device() = 0;
};

Ref<Window> create_window(const Window_Config &config, const Window_Callbacks &callbacks);
