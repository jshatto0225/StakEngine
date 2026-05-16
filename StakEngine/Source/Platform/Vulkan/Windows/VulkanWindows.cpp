#define VK_USE_PLATFORM_WIN32_KHR

#include "VulkanPlatform.h"

#include "Asserts.h"

#define WINDOWS_EXTENSION_COUNT 2

void platform_get_required_extensions(const char **extensions) {
    extensions[0] = VK_KHR_SURFACE_EXTENSION_NAME;
    extensions[1] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

u32 platform_get_required_extension_count() {
    return WINDOWS_EXTENSION_COUNT;
}

bool platform_create_surface(VkInstance instance, void *window_handle, VkSurfaceKHR *surface) {
    assert(instance);
    assert(window_handle);
    assert(surface);

    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hwnd = (HWND) window_handle;
    create_info.hinstance = GetModuleHandle(nullptr);
    VkResult result = vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, surface);
    return result == VK_SUCCESS;
}
