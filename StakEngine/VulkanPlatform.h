#pragma once

#include "Platform.h"

#ifdef SK_VULKAN

#ifdef SK_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#include "Win32Platform.h"
#undef max
#endif

#include <vulkan\vulkan.h>

#include <vector>

#include "Types.h"

const char *ValidationLayers[] = 
{
    "VK_LAYER_KHRONOS_validation"
};

const u32 NumValidationLayers = 1;

const std::vector<const char *> DeviceExtensions = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const u32 DeviceExtensionCount = 1;

#ifdef SK_DEBUG
const bool EnableValidationLayers = true;
#else
const bool EnableValidationLayers = false;
#endif

#endif
