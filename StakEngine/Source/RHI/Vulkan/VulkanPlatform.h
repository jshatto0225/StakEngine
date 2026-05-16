#pragma once

#include <vulkan/vulkan.h>

void platform_get_required_extensions(const char **extensions);
u32 platform_get_required_extension_count();
bool platform_create_surface(VkInstance instance, void *window_handle, VkSurfaceKHR *surface);