#include "rhi.hpp"

#include "vulkan/vulkan.hpp"

#include "utils/utils.hpp"

struct Rhi_Api {
    void(*init)(const String &) = [](const String &app_name) {};
    void(*shutdown)() = []() {};
    
    void *(*create_device)(Platform::Window *) = [](Platform::Window *win) -> void * { return nullptr; };
    void(*destroy_device)(void **) = [](void **device) {};

    void *(*create_command_list)(void *) = [](void *device) -> void * { return nullptr; };
    void (*destroy_command_list)(void **) = [](void **list) {};
};

static Hash_Map<Rhi::Vendor, Rhi_Api> rhi = {
    {
        Rhi::Vendor::NONE,
        {}
    },
    {
        Rhi::Vendor::VULKAN,
        {
            Vulkan::init,
            Vulkan::shutdown,
            
            Vulkan::create_device,
            Vulkan::destroy_device,

            Vulkan::create_command_list,
            Vulkan::destroy_command_list,
        }
    }
};

static Rhi::Vendor active_vendor = Rhi::Vendor::NONE;

void Rhi::init(Vendor vendor, const String &app_name) {
    if (active_vendor != vendor) {
        rhi[active_vendor].shutdown();
        
        active_vendor = vendor;
        rhi[active_vendor].init(app_name);
    }
}

Rhi::Device *Rhi::create_device(Platform::Window *window) {
    return (Device *)rhi[active_vendor].create_device(window);
}

void Rhi::destroy_device(Device **device) {
    rhi[active_vendor].destroy_device((void **)device);
}

Rhi::Command_List *Rhi::create_command_list(Rhi::Device *device) {
    return (Rhi::Command_List *)rhi[active_vendor].create_command_list((void *)device);
}

void Rhi::destroy_command_list(Rhi::Command_List **list) {
    rhi[active_vendor].destroy_command_list((void **)list);
}
