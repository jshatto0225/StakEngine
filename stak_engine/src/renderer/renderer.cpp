#include "renderer.hpp"

static Platform::Window *window = nullptr;

static Rhi::Device *device = nullptr;
static Rhi::Command_List *command_list = nullptr;

static bool initialized = false;

void Renderer::init(Platform::Window *win, Rhi::Vendor vendor, const std::string &app_name) {
	window = win;
    
    Rhi::init(vendor, app_name);

    device = Rhi::create_device(win);
    ASSERT(device);

    command_list = Rhi::create_command_list(device);
    ASSERT(command_list);
}

void Renderer::shutdown() {
    Rhi::destroy_command_list(&command_list);
    Rhi::destroy_device(&device);
    
    initialized = false;
}

void Renderer::update() {
    
}
