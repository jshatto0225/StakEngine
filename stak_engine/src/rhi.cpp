#include "rhi.hpp"

#include "log.hpp"
#include "utils.hpp"

#if defined(SK_WINDOWS)

static const bool vulkan_supported = true;
static const bool directx_supported = false;
static const bool metal_supported = false;

void init_vulkan(const String &s);
void init_directx(const String &s) {}
void init_metal(const String &s) {}

void shutdown_vulkan();
void shutdown_directx() {}
void shutdown_metal() {}

void new_imgui_frame_vulkan();
void new_imgui_frame_directx() {}
void new_imgui_frame_metal() {}

void shutdown_imgui_vulkan();
void shutdown_imgui_directx() {}
void shutdown_imgui_metal() {}

void init_imgui_vulkan(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass);
void init_imgui_directx(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {}
void init_imgui_metal(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {}

Ref<Rhi_Device> create_vulkan_device(Window *w);
Ref<Rhi_Device> create_directx_device(Window *w) { return nullptr; }
Ref<Rhi_Device> create_metal_device(Window *w) { return nullptr; }

#elif defined(SK_LINUX)

static const bool vulkan_supported = true;
static const bool directx_supported = false;
static const bool metal_supported = false;

void init_vulkan(const String &s);
void init_metal(const String &s) {}
void init_directx(const String &s) {}

void shutdown_imgui_vulkan();
void shutdown_imgui_directx() {}
void shutdown_imgui_metal() {}

void shutdown_vulkan();
void shutdown_metal() {}
void shutdown_directx() {}

void new_imgui_frame_vulkan();
void new_imgui_frame_directx() {}
void new_imgui_frame_metal() {}

void init_imgui_vulkan(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass);
void init_imgui_directx(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {}
void init_imgui_metal(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {}

Ref<Rhi_Device> create_vulkan_device(Window *w);
Ref<Rhi_Device> create_directx_device(Window *w) { return nullptr; }
Ref<Rhi_Device> create_metal_device(Window *w) { return nullptr; }

#elif defined(SK_MAC)

static const bool vulkan_supported = false;
static const bool directx_supported = false;
static const bool metal_supported = true;

void init_metal(const String &s);
void init_vulkan(const String &s) {}
void init_directx(const String &s) {}

void shutdown_imgui_metal();
void shutdown_imgui_vulkan() {}
void shutdown_imgui_directx() {}

void shutdown_metal();
void shutdown_vulkan() {}
void shutdown_directx() {}

void new_imgui_frame_metal();
void new_imgui_frame_vulkan() {}
void new_imgui_frame_directx() {}

void init_imgui_metal(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass);
void init_imgui_vulkan(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {}
void init_imgui_directx(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {}

Ref<Rhi_Device> create_metal_device(Window *w);
Ref<Rhi_Device> create_vulkan_device(Window *w) { return nullptr; }
Ref<Rhi_Device> create_directx_device(Window *w) { return nullptr; }

#endif

static Rhi_Vendor active_vendor = Rhi_Vendor::NONE;

static bool platform_supports(Rhi_Vendor vendor) {
    switch (vendor) {
    case Rhi_Vendor::VULKAN:
        return vulkan_supported;
    case Rhi_Vendor::DIRECTX:
        return directx_supported;
    case Rhi_Vendor::METAL:
        return metal_supported;
    }

    return true;
}

void init_rhi(Rhi_Vendor vendor, const String &app_name) {
    if (!platform_supports(vendor)) {
        SK_LOG_WARN("Rendering backend not supported");
        return;
    }
    
    active_vendor = vendor;
    switch (vendor) {
    case Rhi_Vendor::VULKAN:
        init_vulkan(app_name);
        break;
    case Rhi_Vendor::DIRECTX:
        init_directx(app_name);
        break;
    case Rhi_Vendor::METAL:
        init_metal(app_name);
        break;
    }
}

void switch_rhi(Rhi_Vendor vendor, const String &app_name) {
    if (active_vendor == vendor) {
        return;
    }
    
    if (!platform_supports(vendor)) {
        SK_LOG_WARN("Rendering backend not supported");
        return;
    }
    
    switch (active_vendor) {
    case Rhi_Vendor::VULKAN:
        shutdown_vulkan();
        break;
    case Rhi_Vendor::DIRECTX:
        shutdown_directx();
        break;
    case Rhi_Vendor::METAL:
        shutdown_metal();
        break;
    }

    active_vendor = vendor;
    switch (vendor) {
    case Rhi_Vendor::VULKAN:
        init_vulkan(app_name);
        break;
    case Rhi_Vendor::DIRECTX:
        init_directx(app_name);
        break;
    case Rhi_Vendor::METAL:
        init_metal(app_name);
        break;
    }
}

void shutdown_rhi() {
    switch (active_vendor) {
    case Rhi_Vendor::VULKAN:
        shutdown_vulkan();
        break;
    case Rhi_Vendor::DIRECTX:
        shutdown_directx();
        break;
    case Rhi_Vendor::METAL:
        shutdown_metal();
        break;
    }
}

Ref<Rhi_Device> create_rhi_device(Window *window) {
    switch (active_vendor) {
    case Rhi_Vendor::VULKAN:
        return create_vulkan_device(window);
    case Rhi_Vendor::DIRECTX:
        return create_directx_device(window);
    case Rhi_Vendor::METAL:
        return create_metal_device(window);
    }

    return nullptr;
}

void init_imgui_rhi(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {
    switch (active_vendor) {
    case Rhi_Vendor::VULKAN:
        init_imgui_vulkan(window, render_pass);
        break;
    case Rhi_Vendor::DIRECTX:
        init_imgui_directx(window, render_pass);
        break;
    case Rhi_Vendor::METAL:
        init_imgui_metal(window, render_pass);
        break;
    }
}

void shutdown_imgui_rhi() {
    switch (active_vendor) {
    case Rhi_Vendor::VULKAN:
        shutdown_imgui_vulkan();
        break;
    case Rhi_Vendor::DIRECTX:
        shutdown_imgui_directx();
        break;
    case Rhi_Vendor::METAL:
        shutdown_imgui_metal();
        break;
    }
}

void new_imgui_frame_rhi() {
    switch (active_vendor) {
    case Rhi_Vendor::VULKAN:
        new_imgui_frame_vulkan();
        break;
    case Rhi_Vendor::DIRECTX:
        new_imgui_frame_directx();
        break;
    case Rhi_Vendor::METAL:
        new_imgui_frame_metal();
        break;
    }
}
