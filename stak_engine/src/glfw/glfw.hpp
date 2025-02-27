#pragma once

#include "window.hpp"

#include <vulkan/vulkan.h>
#include <glfw/glfw3.h>

struct Window_Data {
	String title = "";
	s32 width = 0;
	s32 height = 0;
	s32 x = 0;
	s32 y = 0;
	Window_Callbacks callbacks = {};
    
    Ref<Rhi_Device> device = {};
};

struct Glfw_Window : public Window {
    Glfw_Window(const Window_Config &config, const Window_Callbacks &callbacks);
    ~Glfw_Window() override;

    void get_size(s32 *width, s32 *height) override;
    void enable_raw_input() override;
    Ref<Rhi_Device> get_device() override;
    void get_vulkan_surface(void *instance, void *surface) override;
    void show_cursor() override;
    void hide_cursor() override;
    void disable_raw_input() override;
    
	GLFWwindow* handle = nullptr;
	Window_Data data = {};
};
