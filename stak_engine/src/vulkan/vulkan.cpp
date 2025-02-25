#include "vulkan.hpp"

#include "log/log.hpp"
#include "utils/utils.hpp"

#include <cstring>

#include <vulkan/vulkan.h>

static const u32 MAX_FRAMES_IN_FLIGHT = 2;

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
};

struct Device {
    Platform::Window *window = nullptr;
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    u32 graphics_queue_index = UINT32_MAX;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    u32 present_queue_index = UINT32_MAX;
    VkQueue present_queue = VK_NULL_HANDLE;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    VkExtent2D extent = {};
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkSurfaceFormatKHR swapchain_format = {};
    Array<VkImage> swapchain_images = {};
    Array<VkImageView> swapchain_image_views = {};
    VkCommandPool command_pool = VK_NULL_HANDLE;

    Array<VkSemaphore> render_finished_semaphores = {};
    Array<VkSemaphore> image_available_semaphores = {};
    Array<VkFence> in_flight_fences = {};

    u32 current_frame = 0;
    u32 image_index = 0;
    bool framebuffer_resized = false;
};

struct Command_List {
    Device *device;

    VkCommandBuffer command_buffers[MAX_FRAMES_IN_FLIGHT];
};

struct Frame_Data {
    Device *device = nullptr;
    Array<Command_List *> command_lists = {};
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

// Hidden from public platform interface
namespace Platform {

Array<const char *> get_vulkan_extensions();
void get_vulkan_surface(Window *window, VkInstance instance, VkSurfaceKHR *surface);

}

static Context context = {};

#ifdef SK_DEBUG
bool ENABLE_VALIDATION_LAYERS = true;
#else
bool ENABLE_VALIDATION_LAYERS = false;
#endif

static VkImageView create_image_view(Device *device, VkImage image, VkFormat format) {
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
    if (vkCreateImageView(device->device, &view_info, nullptr, &image_view) != VK_SUCCESS) {
        SK_LOG_WARN("Failed to create image view");
    }
    
    return image_view;
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

        if (indices.graphics_family.has_value() && indices.present_family.has_value()) {
            break;
        }
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
    if (func != nullptr) {
        return func(instance, info, allocator, messenger);
    } else {
        SK_LOG_ERROR("Extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void create_swapchain(Device *device) {
Swapchain_Support support = get_swapchain_support(device->gpu, device->surface);
    
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
        device->extent = support.capabilities.currentExtent;
    } else {
        s32 width = 0;
        s32 height = 0;
        Platform::get_window_size(device->window, &width, &height);
        
        VkExtent2D extent = { 
            (u32)width, 
            (u32)height
        };
        
        extent.width = std::clamp(extent.width, support.capabilities.minImageExtent.width, support.capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height);
        
        device->extent = extent;
    }
    
    u32 image_count = support.capabilities.minImageCount + 1;
    
    if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount) {
        image_count = support.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = device->surface;
    swapchain_info.minImageCount = image_count;
    swapchain_info.imageFormat = format.format;
    swapchain_info.imageColorSpace = format.colorSpace;
    swapchain_info.imageExtent = device->extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    device->swapchain_format = format;

    Queue_Families indices = get_queue_families(device->gpu, device->surface);
    
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
    if (vkCreateSwapchainKHR(device->device, &swapchain_info, nullptr, &device->swapchain) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create swapchian");
    }
    
    device->swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device->device, device->swapchain, &image_count, device->swapchain_images.data());
    
    device->swapchain_image_views.resize(device->swapchain_images.size());
    
    For (device->swapchain_images.size()) {
        device->swapchain_image_views[i] = create_image_view(device, device->swapchain_images[i], device->swapchain_format.format);
    }
}

static void recreate_swapchain(Device *device) {
    s32 width = 0;
    s32 height = 0;
    Platform::get_window_size(device->window, &width, &height);
    
    while (width == 0 || height == 0) {
        Platform::get_window_size(device->window, &width, &height);
    }
  
    Vulkan::wait_for_device(device);
    
    It (device->swapchain_image_views) {
        vkDestroyImageView(device->device, it, nullptr);
    }
    
    vkDestroySwapchainKHR(device->device, device->swapchain, nullptr);
    
    create_swapchain(device);
}

void Vulkan::init(const String &app_name) {
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

    Array<const char *> extensions = Platform::get_vulkan_extensions();
    
#ifdef SK_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    instance_info.enabledExtensionCount = (u32)extensions.size();
    instance_info.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&instance_info, nullptr, &context.instance) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan instance");
        return;
    }

    if (ENABLE_VALIDATION_LAYERS && create_debug_messenger(context.instance, &debug_messenger_info, nullptr, &context.debug_messenger) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to setup vulkan debug messenger");
        return;
    }
}

void Vulkan::shutdown() {
    vkDestroyInstance(context.instance, nullptr);
    context.instance = VK_NULL_HANDLE;
}

void *Vulkan::create_device(Platform::Window *win) {
    if (win == nullptr) {
        return nullptr;
    }
    
    auto device = new Device;
    
    device->window = win;

    // Surface
    Platform::get_vulkan_surface(win, context.instance, &device->surface);

    // Logical Device
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(context.instance, &device_count, nullptr);
    Array<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(context.instance, &device_count, devices.data());

    It (devices) {
        bool is_device_suitable = false;

        Queue_Families indices = get_queue_families(it, device->surface);

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
            Swapchain_Support support = get_swapchain_support(it, device->surface);
            swapchain_adequate = support.present_modes.size() > 0 && support.formats.size() > 0;
        }

        VkPhysicalDeviceFeatures supported_features;
        vkGetPhysicalDeviceFeatures(it, &supported_features);

        is_device_suitable = extensions_supported && swapchain_adequate && indices.graphics_family.has_value() && indices.present_family.has_value() && supported_features.samplerAnisotropy;

        if (is_device_suitable) {
            device->gpu = it;
            break;
        }
    }

    if (device->gpu == VK_NULL_HANDLE) {
        SK_LOG_WARN("Failed to find physical device");
        delete device;
        return nullptr;
    }

    Queue_Families indices = get_queue_families(device->gpu, device->surface);

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

    if (vkCreateDevice(device->gpu, &device_info, nullptr, &device->device) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan device");
        delete device;
        return nullptr;
    }

    vkGetDeviceQueue(device->device, indices.graphics_family.value(), 0, &device->graphics_queue);
    vkGetDeviceQueue(device->device, indices.present_family.value(), 0, &device->present_queue);

    // Swapchain
    create_swapchain(device);

    // Sync Objects
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    device->render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    device->image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    device->in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

    For (MAX_FRAMES_IN_FLIGHT) {
        if (vkCreateSemaphore(device->device, &semaphore_info, nullptr, &device->image_available_semaphores[i]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            delete device;
            return nullptr;
        }
        
        if (vkCreateSemaphore(device->device, &semaphore_info, nullptr, &device->render_finished_semaphores[i]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            delete device;
            return nullptr;
        }
        
        if (vkCreateFence(device->device, &fence_info, nullptr, &device->in_flight_fences[i]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan fence");
            delete device;
            return nullptr;
        }
    }
    
    // Command Pool
    VkCommandPoolCreateInfo command_pool_info = {};

    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_info.queueFamilyIndex = indices.graphics_family.value();

    if (vkCreateCommandPool(device->device, &command_pool_info, nullptr, &device->command_pool) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan command pool");
        delete device;
        return nullptr;
    }

    return device;
}

void Vulkan::destroy_device(void **d) {
    if (d == nullptr || *d == nullptr) {
        return;
    }

    auto device = (Device *)*d;

    It (device->image_available_semaphores) {
        vkDestroySemaphore(device->device, it, nullptr);
    }
    
    It (device->render_finished_semaphores) {
        vkDestroySemaphore(device->device, it, nullptr);
    }

    It (device->in_flight_fences) {
        vkDestroyFence(device->device, it, nullptr);
    }

    vkDestroyCommandPool(device->device, device->command_pool, nullptr);
    
    It (device->swapchain_image_views) {
        vkDestroyImageView(device->device, it, nullptr);
    }

    vkDestroySwapchainKHR(device->device, device->swapchain, nullptr);

    vkDestroyDevice(device->device, nullptr);

    vkDestroySurfaceKHR(context.instance, device->surface, nullptr);

    delete *d;
    *d = nullptr;
}

void *Vulkan::create_command_list(void *d) {
    if (d == nullptr) {
        return nullptr;
    }

    auto device = (Device *)d;
    auto list = new Command_List;

    list->device = device;

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = device->command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(device->device, &alloc_info, list->command_buffers) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate vulkan command buffers");
        delete list;
        return nullptr;
    }

    return list;
}

void Vulkan::destroy_command_list(void **l) {
    if (l == nullptr || *l == nullptr) {
        return;
    }

    auto list = (Command_List *)*l;
    
    vkFreeCommandBuffers(list->device->device, list->device->command_pool, MAX_FRAMES_IN_FLIGHT, list->command_buffers);
}

void *Vulkan::begin_frame(void *d, const Array<void *> &lists) {
    auto device = (Device *)d;

    auto data = new Frame_Data;
    data->device = device;
    data->command_lists.reserve(lists.size());
    
    It (lists) {
        auto list = (Command_List *)it;

        data->command_lists.push_back(list);
        
        vkResetCommandBuffer(list->command_buffers[list->device->current_frame], 0);
        
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(list->command_buffers[list->device->current_frame], &begin_info) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to begin command buffer");
        }
    }

    vkWaitForFences(device->device, 1, &device->in_flight_fences[device->current_frame], VK_TRUE, UINT64_MAX);

    VkResult err = vkAcquireNextImageKHR(device->device, device->swapchain, UINT64_MAX, device->image_available_semaphores[device->current_frame], VK_NULL_HANDLE, &device->image_index);
    
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain(device);
        delete data;
        begin_frame(d, lists);
    }
    else if (err != VK_SUCCESS && err != VK_SUBOPTIMAL_KHR) {
        SK_LOG_ERROR("Failed to get next swapchain image");
    }
  
    vkResetFences(device->device, 1, &device->in_flight_fences[device->current_frame]);

    return (void *)data;
}

void Vulkan::end_frame(void *frame_data) {
    auto data = (Frame_Data *)frame_data;
    auto device = (Device *)data->device;

    Array<VkCommandBuffer> command_buffers;
    It (data->command_lists) {
        auto list = (Command_List *)it;
        if (list->device != device) {
            SK_LOG_ERROR("list does not belong to device");
        }
        
        command_buffers.push_back(list->command_buffers[device->current_frame]);

        if (vkEndCommandBuffer(list->command_buffers[device->current_frame]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to end command buffer");
        }
    }
    
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &device->image_available_semaphores[device->current_frame];
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &device->render_finished_semaphores[device->current_frame];
    submit_info.commandBufferCount = (u32)command_buffers.size();
    submit_info.pCommandBuffers = command_buffers.data();
    
    if (vkQueueSubmit(device->graphics_queue, 1, &submit_info, device->in_flight_fences[device->current_frame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit to graphics queue");
    }
    
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &device->swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &device->render_finished_semaphores[device->current_frame];
    present_info.pImageIndices = &device->image_index;
    VkResult err = vkQueuePresentKHR(device->present_queue, &present_info);	
  
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || device->framebuffer_resized) {
        device->framebuffer_resized = false;
        recreate_swapchain(device);
    }
    else if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to present");
    }
    
    device->current_frame = (device->current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    delete frame_data;
}

void Vulkan::wait_for_device(void *d) {
    auto device = (Device *)d;
    
    vkDeviceWaitIdle(device->device);
}
