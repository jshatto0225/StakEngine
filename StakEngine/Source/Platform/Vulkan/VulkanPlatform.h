#pragma once

#include "Types.h"

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

std::vector<const char *> platform_get_required_extensions();
bool platform_create_surface(VkInstance instance, void *window_handle, VkSurfaceKHR *surface);
void platform_destroy_surface(VkInstance instance, VkSurfaceKHR surface);