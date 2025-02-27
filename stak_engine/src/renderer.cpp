#include "renderer.hpp"

#include "utils.hpp"
#include "window.hpp"
#include "gui.hpp"

static Ref<Rhi_Device> device = nullptr;
static Ref<Rhi_Command_List> command_list = nullptr;
static Ref<Window> window = nullptr;

void init_renderer(Ref<Window> win, Rhi_Vendor vendor, const String &app_name) {
    init_rhi(vendor, app_name);

    window = win;
    
    device = window->get_device();
    ASSERT(device != nullptr);

    command_list = device->create_command_list();
    ASSERT(command_list != nullptr);

    init_imgui(window);
}

void shutdown_renderer() {
    device->wait_for_idle();
    
    shutdown_imgui();
    shutdown_rhi();
}

void update_renderer() {
    device->new_frame();
    
    command_list->begin();
    {
        // This should always be done last
        render_imgui(command_list);
    }
    command_list->end();

    device->submit({ command_list }, {}, {});
    device->present();
}
