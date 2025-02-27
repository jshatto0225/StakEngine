#include "vulkan.hpp"

#include "rhi.hpp"

#include "log.hpp"
#include "utils.hpp"
#include "window.hpp"
#include "platform.hpp"
#include "glfw/glfw.hpp"

#include <cstring>

#include <vulkan/vulkan.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

static const Array<const char *> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation",
};

static const Array<const char *> DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

struct Context {
    bool initialized = false;
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    Set<Vulkan_Device *> devices = {};
};

struct Queue_Families {
    Option<u32> graphics_family = {};
    Option<u32> present_family = {};
};

struct Swapchain_Support {
    VkSurfaceCapabilitiesKHR capabilities = {};
    Array<VkSurfaceFormatKHR> formats = {};
    Array<VkPresentModeKHR> present_modes = {};
};

static Context context = {};

#ifdef SK_DEBUG

#define CHECK_CONTEXT(...)                             \
    if (!context.initialized) {                        \
        return __VA_ARGS__;                            \
    }

#define CHECK(...)                                      \
    CHECK_CONTEXT(__VA_ARGS__);                         \
    if (!initialized) {                                 \
        return __VA_ARGS__;                             \
    }

#else
#define CHECK_CONTEXT(...)
#define CHECK(...)
#endif

#ifdef SK_DEBUG
bool ENABLE_VALIDATION_LAYERS = true;
#else
bool ENABLE_VALIDATION_LAYERS = false;
#endif

// NOTE: if is_error is false it will print at warning level
static const char *get_err_string(VkResult err) {
    switch (err) {
    case VK_NOT_READY:
        return "VK_NOT_READY";
    case VK_TIMEOUT:
        return "VK_TIMEOUT";
    case VK_EVENT_SET:
        return "VK_EVENT_SET";
    case VK_EVENT_RESET:
        return "VK_EVENT_RESET";
    case VK_INCOMPLETE:
        return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL";
    }

    return "VK_UNKNOWN_ERROR";
}

static Queue_Families get_queue_families(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
    Queue_Families indices = {};

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);
    Array<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_families.data());

    For (queue_family_count) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
        }

        if (indices.graphics_family.has_value() && indices.present_family.has_value()) break;
    }

    return indices;
}

static Swapchain_Support get_swapchain_support(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
    Swapchain_Support support = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &support.capabilities);

    u32 format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, nullptr);
    if (format_count > 0) {
        support.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, support.formats.data());
    }

    u32 present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, nullptr);

    if (present_mode_count > 0) {
        support.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, support.present_modes.data());
    }

    return support;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        SK_LOG_TRACE(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        SK_LOG_INFO(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        SK_LOG_WARN(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        SK_LOG_ERROR(callback_data->pMessage);
        break;        
    }
    
    return VK_FALSE;
}

static VkResult create_debug_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *info, const VkAllocationCallbacks *allocator, VkDebugUtilsMessengerEXT *messenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) return func(instance, info, allocator, messenger);
    else return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroy_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *allocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, messenger, allocator);
    }
}

void init_vulkan(const String &app_name) {
    bool extensions_supported = true;
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    Array<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    ForI (VALIDATION_LAYERS.size()) {
        bool layer_found = false;

        ForJ (layer_count) {
            if (strcmp(VALIDATION_LAYERS[i], available_layers[j].layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            extensions_supported = false;
            break;
        }
    }

    if (ENABLE_VALIDATION_LAYERS && !extensions_supported) {
        SK_LOG_ERROR("Vulkan validation layers not supported");
        return;
    }

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app_name.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "StakEngine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;

    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {};

    if (ENABLE_VALIDATION_LAYERS) {
        instance_info.enabledLayerCount = (u32)VALIDATION_LAYERS.size();
        instance_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        debug_messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_messenger_info.pfnUserCallback = messenger;

        instance_info.pNext = &debug_messenger_info;
    }

    Array<const char *> extensions = get_vulkan_extensions();
    
#ifdef SK_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    instance_info.enabledExtensionCount = (u32)extensions.size();
    instance_info.ppEnabledExtensionNames = extensions.data();

    VkResult err = vkCreateInstance(&instance_info, nullptr, &context.instance);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan instance: {}", get_err_string(err));
        return;
    }

    if (ENABLE_VALIDATION_LAYERS) {
        err = create_debug_messenger(context.instance, &debug_messenger_info, nullptr, &context.debug_messenger);
        if (err != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to setup vulkan debug messenger: {}", get_err_string(err));
            return;
        }
    }

    context.initialized = true;
}

void shutdown_vulkan() {
    Array<Vulkan_Device *> devices = {};
    devices.reserve(context.devices.size());
    It (context.devices) {
        devices.push_back(it);
    }
    It (devices) {
        it->manual_shutdown();
    }
    devices.clear();

    destroy_debug_messenger(context.instance, context.debug_messenger, nullptr);
    vkDestroyInstance(context.instance, nullptr);
    context.instance = VK_NULL_HANDLE;
}

Vulkan_Device::Vulkan_Device(Window *win) {
    if (win == nullptr) {
        SK_LOG_ERROR("Window was nullptr");
        return;
    }

    CHECK_CONTEXT();
    
    window = win;
    
    // Surface
    window->get_vulkan_surface((void *)&context.instance, (void *)&surface);
    
    // Logical Device
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(context.instance, &device_count, nullptr);
    Array<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(context.instance, &device_count, devices.data());
        
    It (devices) {
        bool is_device_suitable = false;
            
        Queue_Families indices = get_queue_families(it, surface);

        u32 device_extension_count;
        vkEnumerateDeviceExtensionProperties(it, nullptr, &device_extension_count, nullptr);
        u32 size = device_extension_count * sizeof(VkExtensionProperties);
        
        Array<VkExtensionProperties> available_extensions(device_extension_count);

        vkEnumerateDeviceExtensionProperties(it, nullptr, &device_extension_count, available_extensions.data());

        bool extensions_supported = false;

        It (available_extensions) {
            if (strcmp(it.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                extensions_supported = true;
                break;
            }
        }

        bool swapchain_adequate = false;
        if (extensions_supported) {
            Swapchain_Support support = get_swapchain_support(it, surface);
            swapchain_adequate = support.present_modes.size() > 0 && support.formats.size() > 0;
        }

        VkPhysicalDeviceFeatures supported_features;
        vkGetPhysicalDeviceFeatures(it, &supported_features);

        is_device_suitable = extensions_supported && swapchain_adequate && indices.graphics_family.has_value() && indices.present_family.has_value() && supported_features.samplerAnisotropy;

        if (is_device_suitable) {
            gpu = it;
            break;
        }
    }

    if (gpu == VK_NULL_HANDLE) {
        SK_LOG_ERROR("Failed to find physical device");
        return;
    }

    Queue_Families indices = get_queue_families(gpu, surface);

    graphics_queue_index = indices.graphics_family.value();
    present_queue_index = indices.present_family.value();

    Set<u32> unique_families = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    Array<VkDeviceQueueCreateInfo> queue_infos = {};
    queue_infos.reserve(unique_families.size());

    f32 queue_priority = 1.0f;
    It (unique_families) {
        VkDeviceQueueCreateInfo queue_info = {};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = it;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &queue_priority;

        queue_infos.push_back(queue_info);
    }

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pQueueCreateInfos = queue_infos.data();
    device_info.queueCreateInfoCount = (u32)queue_infos.size();
    device_info.pEnabledFeatures = &device_features;
    device_info.enabledExtensionCount = (u32)DEVICE_EXTENSIONS.size();
    device_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    if (ENABLE_VALIDATION_LAYERS) {
        device_info.enabledLayerCount = (u32)VALIDATION_LAYERS.size();
        device_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    VkResult err = vkCreateDevice(gpu, &device_info, nullptr, &device);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan device: {}", get_err_string(err));
        return;
    }

    vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
    vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);

    // Swapchain
    create_swapchain();

    // Sync Objects
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

    For (MAX_FRAMES_IN_FLIGHT) {
        err = vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphores[i]);
        if (err != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore: {}", get_err_string(err));
            return;
        }

        err = vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphores[i]);
        if (err != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore: {}", get_err_string(err));
            return;
        }

        err = vkCreateFence(device, &fence_info, nullptr, &in_flight_fences[i]);
        if (err != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan fence: {}", get_err_string(err));
            return;
        }
    }
    
    // Command Pool
    VkCommandPoolCreateInfo command_pool_info = {};

    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_info.queueFamilyIndex = indices.graphics_family.value();

    err = vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan command pool: {}", get_err_string(err));
        return;
    }

    initialized = true;
    context.devices.insert(this);
}

Vulkan_Device::~Vulkan_Device() {
    if (initialized) {
        manual_shutdown();
    }
}

Ref<Rhi_Render_Target> Vulkan_Device::create_swapchain_target(Ref<Rhi_Render_Pass> pass) {
    Rhi_Render_Target_Description desc = {};
    desc.is_swapchain_target = true;
    
    return create_ref<Vulkan_Render_Target>(this, desc, pass);
}

void Vulkan_Device::manual_shutdown() {
    CHECK();

    wait_for_idle();

    Array<Vulkan_Command_List *> lists_to_destroy = {};
    lists_to_destroy.reserve(lists.size());
    It (lists) {
        lists_to_destroy.push_back(it);
    }
    It (lists_to_destroy) {
        it->manual_shutdown();
    }
    lists_to_destroy.clear();

    Array<Vulkan_Sync_Object *> syncs_to_destroy = {};
    syncs_to_destroy.reserve(syncs.size());
    It (syncs) {
        syncs_to_destroy.push_back(it);
    }
    It (syncs_to_destroy) {
        it->manual_shutdown();
    }
    syncs_to_destroy.clear();

    Array<Vulkan_Render_Pass *> passes_to_destroy = {};
    passes_to_destroy.reserve(passes.size());
    It (passes) {
        passes_to_destroy.push_back(it);
    }
    It (passes_to_destroy) {
        it->manual_shutdown();
    }
    passes_to_destroy.clear();

    Array<Vulkan_Render_Target *> targets_to_destroy = {};
    targets_to_destroy.reserve(targets.size());
    It (targets) {
        targets_to_destroy.push_back(it);
    }
    It (targets_to_destroy) {
        it->manual_shutdown();
    }
    targets_to_destroy.clear();

    Array<Vulkan_Render_Target *> backbuffers_to_destroy = {};
    backbuffers_to_destroy.reserve(backbuffers.size());
    It (backbuffers) {
        backbuffers_to_destroy.push_back(it);
    }
    It (backbuffers_to_destroy) {
        it->manual_shutdown();
    }
    backbuffers_to_destroy.clear();
    
    It (image_available_semaphores) {
        vkDestroySemaphore(device, it, nullptr);
    }
    
    It (render_finished_semaphores) {
        vkDestroySemaphore(device, it, nullptr);
    }

    It (in_flight_fences) {
        vkDestroyFence(device, it, nullptr);
    }
    
    vkDestroyCommandPool(device, command_pool, nullptr);
    
    It (swapchain_image_views) {
        vkDestroyImageView(device, it, nullptr);
    }

    vkDestroySwapchainKHR(device, swapchain, nullptr);
    
    vkDestroyDevice(device, nullptr);
    
    vkDestroySurfaceKHR(context.instance, surface, nullptr);

    initialized = false;
    context.devices.erase(this);
}

Ref<Rhi_Command_List> Vulkan_Device::create_command_list() {
    CHECK(nullptr);
    
    return create_ref<Vulkan_Command_List>(this);
}

VkImageView Vulkan_Device::create_image_view(VkImage image, VkFormat format) {
    CHECK_CONTEXT(VK_NULL_HANDLE);
    
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    
    VkImageView image_view = VK_NULL_HANDLE;
    VkResult err = vkCreateImageView(device, &view_info, nullptr, &image_view);
    if (err != VK_SUCCESS) {
        SK_LOG_WARN("Failed to create image view: {}", get_err_string(err));
    }
    
    return image_view;
}

void Vulkan_Device::create_swapchain() {
    CHECK_CONTEXT();
    
    Swapchain_Support support = get_swapchain_support(gpu, surface);
    
    VkSurfaceFormatKHR format = {};
    It (support.formats) {
        if (it.format == VK_FORMAT_B8G8R8A8_SRGB && it.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = it;
            break;
        }
    }
    
    VkPresentModeKHR present_mode = {};
    It (support.present_modes) {
        if (it == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = it;
            break;
        }
    }
    
    if (support.capabilities.currentExtent.width != UINT32_MAX) {
        extent = support.capabilities.currentExtent;
    } else {
        s32 width = 0;
        s32 height = 0;
        window->get_size(&width, &height);
        
        VkExtent2D e = { 
            (u32)width, 
            (u32)height
        };
        
        e.width = std::clamp(e.width, support.capabilities.minImageExtent.width, support.capabilities.maxImageExtent.width);
        e.height = std::clamp(e.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height);
        
        extent = e;
    }
    
    u32 image_count = support.capabilities.minImageCount + 1;
    
    if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount) {
        image_count = support.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = surface;
    swapchain_info.minImageCount = image_count;
    swapchain_info.imageFormat = format.format;
    swapchain_info.imageColorSpace = format.colorSpace;
    swapchain_info.imageExtent = extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    swapchain_format = format;

    Queue_Families indices = get_queue_families(gpu, surface);
    
    u32 queue_family_indices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };
    
    if (indices.graphics_family.value() != indices.present_family.value()) {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    
    swapchain_info.preTransform = support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = VK_NULL_HANDLE;
    VkResult err = vkCreateSwapchainKHR(device, &swapchain_info, nullptr, &swapchain);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create swapchian: {}", get_err_string(err));
        initialized = false;
        return;
    }
    
    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());
    
    swapchain_image_views.resize(swapchain_images.size());
    
    For (swapchain_images.size()) {
        swapchain_image_views[i] = create_image_view(swapchain_images[i], swapchain_format.format);
        if (swapchain_image_views[i] == VK_NULL_HANDLE) {
            initialized = false;
            return;
        }
    }
}

void Vulkan_Device::get_extent(s32 *width, s32 *height) {
    *width  = extent.width;
    *height = extent.height;
}

void Vulkan_Device::recreate_swapchain() {
    CHECK();
    
    s32 width = 0;
    s32 height = 0;
    window->get_size(&width, &height);
    
    while (width == 0 || height == 0)
        window->get_size(&width, &height);
  
    wait_for_idle();
    
    It (swapchain_image_views) {
        vkDestroyImageView(device, it, nullptr);
    }
    
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    
    create_swapchain();

    It (backbuffers) {
        it->destroy_framebuffers();
        it->create_swapchain_framebuffers();
    }
}

void Vulkan_Device::wait_for_idle() {
    CHECK();
    
    vkDeviceWaitIdle(device);
}

void Vulkan_Device::new_frame() {
    CHECK();
    
    VkResult err = vkWaitForFences(device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);
    if (err != VK_SUCCESS) {
        SK_LOG_WARN("Could not wait for fence: {}", get_err_string(err));
        return;
    }

    err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);
    
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        new_frame();
    } else if (err != VK_SUCCESS && err != VK_SUBOPTIMAL_KHR) {
        SK_LOG_ERROR("Failed to get next swapchain image: {}", get_err_string(err));
        initialized = false;
        return;
    }
  
    vkResetFences(device, 1, &in_flight_fences[current_frame]);
}

void Vulkan_Device::submit(const Array<Ref<Rhi_Command_List>> &lists, const Array<Ref<Rhi_Sync_Object>> &waits, const Array<Ref<Rhi_Sync_Object>> &signals) {
    CHECK();
    
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    Array<VkPipelineStageFlags> wait_stages = {};
    Array<VkSemaphore> wait_semaphores = {};
    Array<VkSemaphore> signal_semaphores = {};
    
    if (waits.size() > 0) {
        wait_semaphores.reserve(waits.size());
        wait_stages.reserve(waits.size());

        It (waits) {
            auto wait = (Vulkan_Sync_Object *)it.get();
            if (!wait->initialized) {
                SK_LOG_ERROR("Wait sync object is not initialized");
                return;
            }
            wait_stages.push_back(wait->stages);
            wait_semaphores.push_back(wait->semaphores[current_frame]);
        }
    } else {
        wait_semaphores.push_back(image_available_semaphores[current_frame]);
        wait_stages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    if (signals.size() > 0) {
        signal_semaphores.reserve(signals.size());

        It (signals) {
            auto signal = (Vulkan_Sync_Object *)it.get();
            if (!signal->initialized) {
                SK_LOG_ERROR("Wait signal object is not initialized");
                return;
            }
            signal_semaphores.push_back(signal->semaphores[current_frame]);
        }
    } else {
        signal_semaphores.push_back(render_finished_semaphores[current_frame]);
    }
    
    Array<VkCommandBuffer> command_buffers;
    command_buffers.reserve(lists.size());

    It (lists) {
        auto list = (Vulkan_Command_List *)it.get();
        command_buffers.push_back(list->command_buffers[current_frame]);
    }
    
    submit_info.waitSemaphoreCount = (u32)wait_semaphores.size();
    submit_info.pWaitSemaphores = wait_semaphores.data();
    submit_info.pWaitDstStageMask = wait_stages.data();
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &render_finished_semaphores[current_frame];
    submit_info.commandBufferCount = (u32)command_buffers.size();
    submit_info.pCommandBuffers = command_buffers.data();
    VkResult err = vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame]);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit to graphics queue: {}", get_err_string(err));
        initialized = false;
    }
}

void Vulkan_Device::present() {
    CHECK();
    
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_finished_semaphores[current_frame];
    present_info.pImageIndices = &image_index;
    VkResult err = vkQueuePresentKHR(present_queue, &present_info);	
  
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || framebuffer_resized) {
        framebuffer_resized = false;
        recreate_swapchain();
    } else if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to present: {}", get_err_string(err));
        initialized = false;
        return;
    }
    
    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

Ref<Rhi_Sync_Object> Vulkan_Device::create_sync_object(Rhi_Pipeline_Stage_Mask wait_stages) {
    CHECK(nullptr);

    return create_ref<Vulkan_Sync_Object>(this, wait_stages);
}

Ref<Rhi_Render_Pass> Vulkan_Device::create_render_pass(const Rhi_Render_Pass_Description &desc) {
    CHECK(nullptr);

    return create_ref<Vulkan_Render_Pass>(this, desc);
}

Rhi_Format Vulkan_Device::get_swapchain_image_format() {
    CHECK(RHI_FORMAT_UNDEFINED);
    
    return (Rhi_Format)swapchain_format.format;
}

void Vulkan_Device::notify_window_resize()  {
    CHECK();
    
    framebuffer_resized = true;
}

Vulkan_Command_List::Vulkan_Command_List(Vulkan_Device *d) {
    CHECK_CONTEXT();
    
    if (d == nullptr || !d->initialized) {
        SK_LOG_ERROR("Tried to create command list with uninitialized device");
        return;
    }
    
    device = d;
    
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = device->command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    auto err = vkAllocateCommandBuffers(device->device, &alloc_info, command_buffers);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate vulkan command buffers: {}", get_err_string(err));
        return;
    }

    initialized = true;

    device->lists.insert(this);
}

Vulkan_Command_List::~Vulkan_Command_List() {
    if (initialized) {
        manual_shutdown();
    }
}

void Vulkan_Command_List::manual_shutdown() {
    CHECK();

    vkFreeCommandBuffers(device->device, device->command_pool, MAX_FRAMES_IN_FLIGHT, command_buffers);
    initialized = false;

    device->lists.erase(this);
}

void Vulkan_Command_List::begin() {
    CHECK();
    
    vkResetCommandBuffer(command_buffers[device->current_frame], 0);
        
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    auto err = vkBeginCommandBuffer(command_buffers[device->current_frame], &begin_info);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to begin command buffer: {}", get_err_string(err));
        initialized = false;
    }
}

void Vulkan_Command_List::end() {
    CHECK();

    auto err = vkEndCommandBuffer(command_buffers[device->current_frame]);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to end command buffer: {}", get_err_string(err));
        initialized = false;
    }
}

void Vulkan_Command_List::begin_render_pass(Ref<Rhi_Render_Pass> pass, Rhi_Subpass_Contents contents, const Rhi_Render_Pass_Info &info) {
    CHECK();
    
    if (active_render_pass != nullptr) {
        SK_LOG_ERROR("Previous render pass not ended");
        initialized = false;
        return;
    }

    if (pass == nullptr) {
        SK_LOG_ERROR("render pass was nullptr");
        initialized = false;
        return;
    }

    if (info.target == nullptr) {
        SK_LOG_ERROR("render target was nullptr");
        initialized = false;
        return;
    }

    if (info.clear_values.size() == 0) {
        SK_LOG_ERROR("No clear values provided");
        initialized = false;
        return;
    }

    active_render_pass = ptr_cast<Vulkan_Render_Pass>(pass);
    subpass_index = 0;

    VkRenderPassBeginInfo begin_info = {};
    
    begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin_info.renderPass = active_render_pass->render_pass;
    auto target = ptr_cast<Vulkan_Render_Target>(info.target);
    if (target->is_swapchain_backbuffer) {
        begin_info.framebuffer = target->framebuffers[device->image_index];
    } else {
        SK_LOG_ERROR("Only swapchain backbuffers are supported currently");
        initialized = false;
        return;
    }

    VkRect2D rect = {};
    rect.offset = { info.render_area.x,     info.render_area.y };
    rect.extent = { info.render_area.width, info.render_area.height };
    
    begin_info.renderArea = rect;

    Array<VkClearValue> clear_values = {};
    clear_values.reserve(info.clear_values.size());

    It (info.clear_values) {
        VkClearValue val = {};
        
        switch (it.format) {
        case RHI_FORMAT_R8G8B8A8_UNORM:
        case RHI_FORMAT_B8G8R8A8_UNORM:
        case RHI_FORMAT_R16G16B16A16_UNORM:
        case RHI_FORMAT_R16G16B16A16_SNORM:
        case RHI_FORMAT_R16G16B16A16_SFLOAT:
        case RHI_FORMAT_R32G32B32A32_SFLOAT:
            val.color.float32[0] = it.clear_color[0];
            val.color.float32[1] = it.clear_color[1];
            val.color.float32[2] = it.clear_color[2];
            val.color.float32[3] = it.clear_color[3];
            break;
        case RHI_FORMAT_R8G8B8A8_UINT:
        case RHI_FORMAT_R8G8B8A8_SINT:
        case RHI_FORMAT_R16G16B16A16_UINT:
        case RHI_FORMAT_R16G16B16A16_SINT:
        case RHI_FORMAT_R32G32B32A32_UINT:
        case RHI_FORMAT_R32G32B32A32_SINT:
            val.color.uint32[0] = (u32)it.clear_color[0];
            val.color.uint32[1] = (u32)it.clear_color[1];
            val.color.uint32[2] = (u32)it.clear_color[2];
            val.color.uint32[3] = (u32)it.clear_color[3];
            break;
        case RHI_FORMAT_D32_SFLOAT:
            val.depthStencil.depth = it.depth_stencil.depth;
            break;
        case RHI_FORMAT_D24_UNORM_S8_UINT:
        case RHI_FORMAT_D32_SFLOAT_S8_UINT:
            val.depthStencil.depth = it.depth_stencil.depth;
            val.depthStencil.stencil = it.depth_stencil.stencil;
            break;
        default:
            SK_LOG_ERROR("Unsopported format for clear value");
            initialized = false;
            return;
        }

        clear_values.push_back(val);
    }
    
    begin_info.clearValueCount = (u32)clear_values.size();    
    begin_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffers[device->current_frame], &begin_info, (VkSubpassContents)contents);
}

void Vulkan_Command_List::next_subpass(Rhi_Subpass_Contents contents) {
    CHECK();

    if (active_render_pass == nullptr) {
        SK_LOG_ERROR("There is no active render pass");
        initialized = false;
        return;
    }

    if (!active_render_pass->initialized) {
        SK_LOG_ERROR("Active renderpass has error");
        initialized = false;
        return;
    }
    
    if (subpass_index == active_render_pass->subpass_count) {
        SK_LOG_ERROR("No more subpasses in this render pass");
        initialized = false;
        return;
    }

    vkCmdNextSubpass(command_buffers[device->current_frame], (VkSubpassContents)contents);
}

void Vulkan_Command_List::end_render_pass() {
    CHECK();
    
    vkCmdEndRenderPass(command_buffers[device->current_frame]);
    
    active_render_pass = nullptr;
}

void Vulkan_Command_List::render_imgui_draw_data(ImDrawData *data) {
    ImGui_ImplVulkan_RenderDrawData(data, command_buffers[device->current_frame]);
}

Vulkan_Sync_Object::Vulkan_Sync_Object(Vulkan_Device *dev, Rhi_Pipeline_Stage_Mask s) {
    CHECK_CONTEXT();

    if (s == 0) {
        SK_LOG_ERROR("Invalid pipeline stage");
    }
    
    device = dev;
    stages = s;
    
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    It (semaphores) {
        VkResult err = vkCreateSemaphore(device->device, &semaphore_info, nullptr, &it);
        if (err != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create semaphore: {}", get_err_string(err));
            return;
        }
    }   

    initialized = true;

    device->syncs.insert(this);
}

Vulkan_Sync_Object::~Vulkan_Sync_Object() {
    if (initialized)
        manual_shutdown();
}

void Vulkan_Sync_Object::manual_shutdown() {
    CHECK();

    It (semaphores) {
        vkDestroySemaphore(device->device, it, nullptr);
    }

    initialized = false;
    
    device->syncs.erase(this);
}

Vulkan_Render_Pass::Vulkan_Render_Pass(Vulkan_Device *dev, const Rhi_Render_Pass_Description &desc) {
    CHECK_CONTEXT();

    device = dev;
    subpass_count = (u32)desc.subpasses.size();

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    Array<VkAttachmentDescription> attachments = {};
    Array<VkSubpassDescription> subpasses = {};
    Array<VkSubpassDependency> deps = {};
    
    Array<Array<VkAttachmentReference>> input_refs = {};
    Array<Array<VkAttachmentReference>> color_refs = {};
    Array<Array<VkAttachmentReference>> resolve_refs = {};
    Array<VkAttachmentReference> depth_stencil_refs = {};

    It (desc.attachments) {
        VkAttachmentDescription attachment = {};
        
        if (it.may_alias) {
            attachment.flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
        }

        attachment.format = (VkFormat)it.format;
        attachment.samples = (VkSampleCountFlagBits)it.samples;
        attachment.loadOp = (VkAttachmentLoadOp)it.load_op;
        attachment.storeOp = (VkAttachmentStoreOp)it.store_op;
        attachment.stencilLoadOp = (VkAttachmentLoadOp)it.stencil_load_op;
        attachment.stencilStoreOp = (VkAttachmentStoreOp)it.stencil_store_op;
        attachment.initialLayout = (VkImageLayout)it.initial_layout;
        attachment.finalLayout = (VkImageLayout)it.final_layout;

        attachments.push_back(attachment);
    }

    For (desc.subpasses.size()) {
        input_refs.push_back({});
        color_refs.push_back({});
        resolve_refs.push_back({});
        
        const Rhi_Subpass &it = desc.subpasses[i];
        
        VkSubpassDescription subpass = {};

        subpass.pipelineBindPoint = (VkPipelineBindPoint)it.pipeline_bind_point;
        
        It (it.input_attachments) {
            VkAttachmentReference ref = {};

            ref.attachment = it.attachment;
            ref.layout = (VkImageLayout)it.layout;

            input_refs[i].push_back(ref);
        }
        
        subpass.inputAttachmentCount = (u32)input_refs[i].size();
        subpass.pInputAttachments = input_refs[i].data();

        It (it.color_attachments) {
            VkAttachmentReference ref = {};

            ref.attachment = it.attachment;
            ref.layout = (VkImageLayout)it.layout;

            color_refs[i].push_back(ref);
        }

        subpass.colorAttachmentCount = (u32)color_refs[i].size();
        subpass.pColorAttachments = color_refs[i].data();

        It (it.resolve_attachments) {
            VkAttachmentReference ref = {};

            ref.attachment = it.attachment;
            ref.layout = (VkImageLayout)it.layout;

            resolve_refs[i].push_back(ref);
        }

        subpass.pResolveAttachments = resolve_refs[i].data();

        if (it.depth_stencil_attachment.has_value()) {
            Rhi_Attachment_Ref ref = it.depth_stencil_attachment.value();
            VkAttachmentReference vkref = {};
            
            vkref.attachment = ref.attachment;
            vkref.layout = (VkImageLayout)ref.layout;

            depth_stencil_refs.push_back(vkref);
            
            subpass.pDepthStencilAttachment = &depth_stencil_refs.back();
        } else {
            // NOTE: Push empty ref to maintain correct index
            depth_stencil_refs.push_back({});
        }

        subpass.preserveAttachmentCount = (u32)it.preserve_attachments.size();
        subpass.pPreserveAttachments = it.preserve_attachments.data();

        subpasses.push_back(subpass);
    }

    It (desc.deps) {
        VkSubpassDependency dep = {};

        dep.srcSubpass = it.src_subpass;
        dep.dstSubpass = it.dst_subpass;
        dep.srcStageMask = (VkPipelineStageFlags)it.src_stages;
        dep.dstStageMask = (VkPipelineStageFlags)it.dst_stages;
        dep.srcAccessMask = (VkAccessFlags)it.src_access;
        dep.dstAccessMask = (VkAccessFlags)it.dst_access;

        deps.push_back(dep);
    }

    info.attachmentCount = (u32)attachments.size();
    info.pAttachments = attachments.data();

    info.subpassCount = (u32)subpasses.size();
    info.pSubpasses = subpasses.data();

    info.dependencyCount = (u32)deps.size();
    info.pDependencies = deps.data();

    VkResult err = vkCreateRenderPass(device->device, &info, nullptr, &render_pass);
    if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create render pass: {}", get_err_string(err));
        return;
    }

    initialized = true;
    device->passes.insert(this);
}

Vulkan_Render_Pass::~Vulkan_Render_Pass() {
    if (initialized) {
        manual_shutdown();
    }
}

void Vulkan_Render_Pass::manual_shutdown() {
    CHECK();

    vkDestroyRenderPass(device->device, render_pass, nullptr);
    initialized = false;

    device->passes.erase(this);
}


Vulkan_Render_Target::Vulkan_Render_Target(Vulkan_Device *dev, const Rhi_Render_Target_Description &desc, Ref<Rhi_Render_Pass> pass) {
    CHECK_CONTEXT();

    if (pass == nullptr) {
        SK_LOG_ERROR("No render pass given to render target");
        return;
    }

    active_pass = ptr_cast<Vulkan_Render_Pass>(pass);

    if (!active_pass->initialized) {
        SK_LOG_ERROR("Cannot create render target with uninitialized render pass");
        return;
    }
    
    device = dev;
    
    if (!desc.is_swapchain_target) {
        SK_LOG_ERROR("Non swapchain render targets are not supported");
    } else {
        is_swapchain_backbuffer = true;
        create_swapchain_framebuffers();
    }
}

Vulkan_Render_Target::~Vulkan_Render_Target() {
    if (initialized)
        manual_shutdown();
}

void Vulkan_Render_Target::destroy_framebuffers() {
    CHECK();
    
    It (framebuffers) {
        vkDestroyFramebuffer(device->device, it, nullptr);
    }

    initialized = false;
}

void Vulkan_Render_Target::create_swapchain_framebuffers() {    
    if (!active_pass->initialized) {
        SK_LOG_ERROR("Cannot recreate render target with uninitialized render pass");
        initialized = false;
        return;
    }
    
    if (initialized) {
        destroy_framebuffers();
    }

    framebuffers.resize(device->swapchain_images.size());
    
    For (device->swapchain_images.size()) {
        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

        info.renderPass = active_pass->render_pass;
        info.attachmentCount = 1;
        info.pAttachments = &device->swapchain_image_views[i];
        info.width = device->extent.width;
        info.height = device->extent.height;
        info.layers = 1;
        
        VkResult err = vkCreateFramebuffer(device->device, &info, nullptr, &framebuffers[i]);
        if (err != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create swapchain render target");
            return;
        }
    }

    initialized = true;
    device->backbuffers.insert(this);
}

void Vulkan_Render_Target::manual_shutdown() {
    CHECK();

    destroy_framebuffers();

    if (!is_swapchain_backbuffer) {
        // TODO:
        device->targets.erase(this);
    } else {
        device->backbuffers.erase(this);
    }

    active_pass = nullptr;
    
    initialized = false;
}

Ref<Rhi_Device> create_vulkan_device(Window *window) {
    auto device = create_ref<Vulkan_Device>(window);
    return device;
}

void init_imgui_vulkan(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass) {
    ImGui_ImplGlfw_InitForVulkan(ptr_cast<Glfw_Window>(window)->handle, true);

    auto device = ptr_cast<Vulkan_Device>(window->get_device());
    
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = context.instance;
    init_info.PhysicalDevice = device->gpu;
    init_info.Device = device->device;
    init_info.QueueFamily = device->graphics_queue_index;
    init_info.Queue = device->graphics_queue;
    init_info.RenderPass = ptr_cast<Vulkan_Render_Pass>(render_pass)->render_pass;
    init_info.MinImageCount = (u32)device->swapchain_images.size();
    init_info.ImageCount = (u32)device->swapchain_images.size();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.DescriptorPoolSize = 4000;
    init_info.CheckVkResultFn = [](VkResult err) {
        if (err != VK_SUCCESS) {
            SK_LOG_ERROR("ImGui Vulkan Error: {}", get_err_string(err));
        }
    };
    init_info.MinAllocationSize = 1024 * 1024;
    
    ImGui_ImplVulkan_Init(&init_info);
}

void shutdown_imgui_vulkan() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void new_imgui_frame_vulkan() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}
