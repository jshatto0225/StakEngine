#include "VulkanRHI.h"

#include "FLog.h"
#include "FApplication.h"
#include "VulkanRHICommandContext.h"

#include <set>

#ifdef SK_GLFW
#include "FGLFWWindow.h"
#endif

#define CHECK_VK_ERR(Err, Message) \
if (Err != VK_SUCCESS) {           \
    SK_LOG_ERROR(Message);         \
}

VKAPI_ATTR VkBool32 VKAPI_CALL VkMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        SK_LOG_ERROR(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        SK_LOG_INFO(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        SK_LOG_INFO(callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        SK_LOG_WARN(callback_data->pMessage);
        break;
    default:
        break;
    }

    return VK_FALSE;
}

VkResult CreateDebugMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *info, const VkAllocationCallbacks *allocator, VkDebugUtilsMessengerEXT *messenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, info, allocator, messenger);
    }
    else {
        SK_LOG_ERROR("Extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *allocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, messenger, allocator);
    }
}

Vulkan_Rhi::Queue_Families Vulkan_Rhi::find_queue_families(VkPhysicalDevice device) {
    Queue_Families indices;
    
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    
    for (u32 queue_family_index = 0; queue_family_index < queue_family_count; queue_family_index++) {
        if (queue_families[queue_family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = queue_family_index;
        }
        
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, queue_family_index, surface, &present_support);
        if (present_support) {
            indices.present_family = queue_family_index;
        }
        if (indices.present_family.has_value() && indices.graphics_family.has_value()) {
            break;
        }
    }
    
    return indices;
}

Vulkan_Rhi::Swapchain_Support Vulkan_Rhi::get_swapchain_support(VkPhysicalDevice device) {
    Swapchain_Support support = {};
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);
    
    u32 format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, NULL);
    support.formats.resize(format_count);
    
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, support.formats.data());
    
    u32 present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, NULL);
    support.present_modes.resize(present_mode_count);
    
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, support.present_modes.data());
    
    return support;
}

Vulkan_Rhi::Vulkan_Rhi() {
    command_context = new Vulkan_Rhi_Command_Context();
    
    // INSTANCE
    bool extensions_supported = true;
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);
    
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    
    for (u32 validation_layer_index = 0; validation_layer_index < VALIDATION_LAYERS.size(); validation_layer_index++) {
        bool layer_found = false;
        
        for (u32 layerindex = 0; layerindex < layer_count; layerindex++) {
            if (strcmp(VALIDATION_LAYERS[validation_layer_index], available_layers[layerindex].layerName)) {
                layer_found = true;
            }
        }
        
        if (!layer_found) {
            extensions_supported = false;
            break;
        }
    }
    
    if (ENABLE_VALIDATION_LAYERS && !extensions_supported) {
        SK_LOG_ERROR("Validation layers not supported");
    }
    
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = App::get().get_name();
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "StakEngine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;
    
    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {};
    
    if (ENABLE_VALIDATION_LAYERS) {
        instance_info.enabledLayerCount = static_cast<u32>(VALIDATION_LAYERS.size());
        instance_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        
        debug_messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_messenger_info.pfnUserCallback = VkMessenger;
        
        instance_info.pNext = &debug_messenger_info;
    }

#ifdef SK_GLFW
    u32 glfw_extension_count = 0;
    const char **glfw_extensions = NULL;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    
    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);
#else
    std::vector<const char *> extensions;
#endif

    if (ENABLE_VALIDATION_LAYERS) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instance_info.enabledExtensionCount = static_cast<u32>(extensions.size());
    instance_info.ppEnabledExtensionNames = extensions.data();

    CHECK_VK_ERR(vkCreateInstance(&instance_info, NULL, &instance), "Failed to create vulkan instance");
    CHECK_VK_ERR(CreateDebugMessenger(instance, &debug_messenger_info, NULL, &debug_messenger), "Failed to create vulkan debug messenger");

    // SURFACE
#ifdef SK_GLFW
    Ref<Glfw_Window> glfw_window = std::static_pointer_cast<Glfw_Window>(App::get().get_window());
    glfwCreateWindowSurface(instance, glfw_window->get_glfw_window(), NULL, &surface);
#endif

    // DEVICE
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    bool device_found = false;
    for (u32 device_index = 0; device_index < device_count; device_index++) {
        bool is_device_suitable = false;
        Queue_Families Indices = find_queue_families(devices[device_index]);
        
        u32 device_extension_count = 0;
        vkEnumerateDeviceExtensionProperties(devices[device_index], NULL, &device_extension_count, NULL);
        std::vector<VkExtensionProperties> available_extensions(device_extension_count);
        
        vkEnumerateDeviceExtensionProperties(devices[device_index], NULL, &device_extension_count, available_extensions.data());
        
        bool extensions_supported = false;
        
        for (u32 device_extension_index = 0; device_extension_index < device_extension_count; device_extension_index++) {
            if (std::strcmp(available_extensions[device_extension_index].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
                extensions_supported = true;
                break;
            }
        }
        
        bool swapchain_adequate = false;
        if (extensions_supported) {
            Swapchain_Support support = get_swapchain_support(devices[device_index]);
            swapchain_adequate = support.present_modes.size() > 0 && support.formats.size() > 0;
        }

        VkPhysicalDeviceFeatures supported_features;
        vkGetPhysicalDeviceFeatures(devices[device_index], &supported_features);

        is_device_suitable = extensions_supported && swapchain_adequate && Indices.graphics_family.has_value() && Indices.present_family.has_value() && supported_features.samplerAnisotropy;

        if (is_device_suitable) {
            gpu = devices[device_index];
            device_found = true;
            break;
        }
    }

    if (!device_found) {
        SK_LOG_ERROR("Failed to find suitable physical device");
    }

    Queue_Families indices = find_queue_families(gpu);

    std::set<u32> unique_families = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };

    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    queue_infos.reserve(unique_families.size());

    f32 queue_priority = 1.0f;
    for (u32 queue_family : unique_families) {
        VkDeviceQueueCreateInfo queue_info = {};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = queue_family;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &queue_priority;
        queue_infos.push_back(queue_info);
    }

    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features = {};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;
    device_info.pNext = &dynamic_rendering_features;

    VkPhysicalDeviceMultiviewFeatures multiview_features = {};
    multiview_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
    multiview_features.multiview = VK_TRUE;

    dynamic_rendering_features.pNext = &multiview_features;
    
    VkPhysicalDeviceSynchronization2Features sync2_features = {};
    sync2_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
    sync2_features.synchronization2 = VK_TRUE;
    
    multiview_features.pNext = &sync2_features;
    
    device_info.queueCreateInfoCount = static_cast<u32>(queue_infos.size());
    device_info.pQueueCreateInfos = queue_infos.data();
    device_info.pEnabledFeatures = &device_features;
    device_info.enabledExtensionCount = static_cast<u32>(DEVICE_EXTENSIONS.size());
    device_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    if (ENABLE_VALIDATION_LAYERS) {
        device_info.enabledLayerCount = static_cast<u32>(VALIDATION_LAYERS.size());
        device_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    CHECK_VK_ERR(vkCreateDevice(gpu, &device_info, NULL, &device), "Failed to create vulkan device");

    vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
    vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);

    graphics_queue_index = indices.graphics_family.value();
    present_queue_index = indices.present_family.value();

    // SWAPCHAIN
    create_swapchain();
    create_image_views();
    
    // SYNC
    create_sync_objects();

    // IMGUI
    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };
    
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = static_cast<u32>(std::size(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;

    CHECK_VK_ERR(vkCreateDescriptorPool(device, &pool_info, NULL, &imgui_pool), "Failed to create imgui descriptor pool");

    // COMMAND BUFFERS
    VkCommandPoolCreateInfo command_pool_info = {};

    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_info.queueFamilyIndex = graphics_queue_index;

    CHECK_VK_ERR(vkCreateCommandPool(device, &command_pool_info, NULL, &command_pool), "Failed to create vulkan command pool");

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
    CHECK_VK_ERR(vkAllocateCommandBuffers(device, &alloc_info, command_buffers.data()), "Failed to allocate vulkan command buffers");
}

Vulkan_Rhi::~Vulkan_Rhi() {
    delete command_context;
    
    for (VkSemaphore semaphore : image_available_semaphores) {
        vkDestroySemaphore(device, semaphore, NULL);
    }
    
    for (VkSemaphore semaphore : render_finished_semaphores) {
        vkDestroySemaphore(device, semaphore, NULL);
    }

    for (VkFence fence : in_flight_fences) {
        vkDestroyFence(device, fence, NULL);
    }

    vkFreeCommandBuffers(device, command_pool, static_cast<u32>(command_buffers.size()), command_buffers.data());
    vkDestroyCommandPool(device, command_pool, NULL);

    vkDestroyDescriptorPool(device, imgui_pool, NULL);

    for (u32 image_index = 0; image_index < swapchain_images.size(); image_index++) {
        vkDestroyImageView(device, swapchain_image_views[image_index], NULL);
    }

    vkDestroySwapchainKHR(device, swapchain, NULL);

    vkDestroyDevice(device, NULL);

    vkDestroySurfaceKHR(instance, surface, NULL);

    DestroyDebugMessenger(instance, debug_messenger, NULL);

    vkDestroyInstance(instance, NULL);
}

void Vulkan_Rhi::create_sync_objects() {
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        CHECK_VK_ERR(vkCreateSemaphore(device, &semaphore_info, NULL, &image_available_semaphores[i]), "Failed to create vulkan semaphore");
        
        CHECK_VK_ERR(vkCreateSemaphore(device, &semaphore_info, NULL, &render_finished_semaphores[i]), "Failed to create vulkan semaphore");
        
        CHECK_VK_ERR(vkCreateFence(device, &fence_info, NULL, &in_flight_fences[i]), "Failed to create vulkan fence");
    }
}

void Vulkan_Rhi::recreate_swapchain() {
    // TODO
    s32 width;
    s32 height;
    App::get().get_window()->get_size(&width, &height);
    
    while (width == 0 || height == 0) {
        App::get().get_window()->get_size(&width, &height);
    }
  
    wait_for_gpu_idle();
    
    for (u32 image_index = 0; image_index < swapchain_images.size(); image_index++) {
        vkDestroyImageView(device, swapchain_image_views[image_index], NULL);
    }
    
    vkDestroySwapchainKHR(device, swapchain, NULL);
    
    create_swapchain();
    create_image_views();
}

void Vulkan_Rhi::create_swapchain() {
    Swapchain_Support support = get_swapchain_support(gpu);
    
    VkSurfaceFormatKHR format = {};
    for (u32 format_index = 0; format_index < support.formats.size(); format_index++) {
        if (support.formats[format_index].format == VK_FORMAT_B8G8R8A8_SRGB && support.formats[format_index].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = support.formats[format_index];
            break;
        }
    }
    
    VkPresentModeKHR present_mode = {};
    for (u32 present_mode_index = 0; present_mode_index < support.present_modes.size(); present_mode_index++) {
        if (support.present_modes[present_mode_index] == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = support.present_modes[present_mode_index];
            break;
        }
    }
    
    if (support.capabilities.currentExtent.width != UINT32_MAX) {
        extent = support.capabilities.currentExtent;
    }
    else {
        s32 width;
        s32 height;
        App::get().get_window()->get_size(&width, &height);
        
        VkExtent2D extent = { 
            static_cast<u32>(width), 
            static_cast<u32>(height) 
        };
        
        extent.width = std::clamp(extent.width, support.capabilities.minImageExtent.width, support.capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height);
        
        this->extent = extent;
    }
    
    u32 image_count = support.capabilities.maxImageCount + 1;
    
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
    
    swapchain_image_format = format;
    
    Queue_Families indices = find_queue_families(gpu);
    
    u32 queue_family_indices[] = {
        indices.graphics_family.value(),
        indices.present_family.value()
    };
    
    if (indices.graphics_family.value() != indices.present_family.value()) {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = queue_family_indices;
    }
    else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    
    swapchain_info.preTransform = support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = VK_NULL_HANDLE;
    CHECK_VK_ERR(vkCreateSwapchainKHR(device, &swapchain_info, NULL, &swapchain), "Failed to create swapchian");
    
    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, swapchain, &image_count, swapchain_images.data());
}

VkImageView Vulkan_Rhi::create_image_view(VkImage image, VkFormat format) {
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
    
    VkImageView image_view;
    CHECK_VK_ERR(vkCreateImageView(device, &view_info, NULL, &image_view), "Failed to create image view");
    
    return image_view;
}

void Vulkan_Rhi::create_image_views() {
    swapchain_image_views.resize(swapchain_images.size());
    
    for (u32 image_view_index = 0; image_view_index < swapchain_images.size(); image_view_index++) {
        swapchain_image_views[image_view_index] = create_image_view(swapchain_images[image_view_index], swapchain_image_format.format);
    }
}

Rhi_Sampler_State_Ref Vulkan_Rhi::create_sampler_state(const Rhi_Sampler_State_Initializer &Initializer) {
    return NULL;
}

Rhi_Rasterizer_State_ref Vulkan_Rhi::create_rasterizer_state(const Rhi_Rasterizer_State_Initializer &Initializer) {
    return NULL;
}

Rhi_Depth_Stencil_State_Ref Vulkan_Rhi::create_depth_stencil_state(const Rhi_Depth_Stencil_State_Initializer &Initializer) {
    return NULL;
}

Rhi_Blend_State_Ref Vulkan_Rhi::create_blend_state(const Rhi_Blend_State_Initializer &Initializer) {
    return NULL;
}

Rhi_Vertex_Declaration_Ref Vulkan_Rhi::create_vertex_declaration(const Rhi_Vertex_Declaration_Element_List &Elements) {
    return NULL;
}

Rhi_Pixel_Shader_Ref Vulkan_Rhi::create_pixel_shader() {
    return NULL;
}

Rhi_Vertex_Shader_Ref Vulkan_Rhi::create_vertex_shader() {
    return NULL;
}

Rhi_Geometry_Shader_Ref Vulkan_Rhi::create_geometry_shader() {
    return NULL;
}

Rhi_Buffer_Ref Vulkan_Rhi::create_buffer(const Rhi_Buffer_Description &Description, Rhi_Access Access) {
    return NULL;
}

Rhi_Uniform_Buffer_Ref Vulkan_Rhi::create_uniform_buffer(const void *InitialContents, const Rhi_Uniform_Buffer_Layout &Layout, Rhi_Uniform_Buffer_Usage Usage) {
    return NULL;
}

Rhi_Staging_Buffer_Ref Vulkan_Rhi::create_staging_buffer() {
    return NULL;
}

Rhi_Texture_Ref Vulkan_Rhi::create_texture(const Rhi_Texture_Description &Description) {
    return NULL;
}

Rhi_Graphics_Pipeline_State_Ref Vulkan_Rhi::create_graphics_pipeline_state(const Rhi_Graphics_Pipeline_State_Initializer &Initializer) {
    return NULL;
}

Rhi_Shader_Resource_View_Ref Vulkan_Rhi::create_shader_resource_view(Rhi_Viewable_Resource_Ref Resource, const Rhi_View_Description &Description) {
    return NULL;
}

Rhi_Unordered_Access_View_Ref Vulkan_Rhi::create_unordered_access_view(Rhi_Viewable_Resource_Ref Resource, const Rhi_View_Description &Description) {
    return NULL;
}

void Vulkan_Rhi::init_imgui() {
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Allocator = NULL;
    init_info.CheckVkResultFn = [](VkResult err) {
        CHECK_VK_ERR(err, "ImGui Vulkan Error");
    };
    init_info.DescriptorPool = imgui_pool;
    init_info.Instance = instance;
    init_info.Device = device;
    init_info.ImageCount = static_cast<u32>(swapchain_images.size());
    init_info.MinImageCount = static_cast<u32>(swapchain_images.size());
    init_info.MinAllocationSize = 1024 * 1024;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.PhysicalDevice = gpu;
    init_info.PipelineCache = NULL;
    VkPipelineRenderingCreateInfo PipelineInfo = {};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    PipelineInfo.viewMask = 0x01;
    PipelineInfo.colorAttachmentCount = 1;
    VkFormat Formats[] = { swapchain_image_format.format };
    PipelineInfo.pColorAttachmentFormats = Formats;
    PipelineInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    PipelineInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
    init_info.PipelineRenderingCreateInfo = PipelineInfo;
    init_info.Queue = graphics_queue;
    init_info.QueueFamily = graphics_queue_index;
    init_info.RenderPass = VK_NULL_HANDLE;
    init_info.Subpass = 0;
    init_info.UseDynamicRendering = true;
    
    ImGui_ImplVulkan_Init(&init_info);
}

void Vulkan_Rhi::shutdown_imgui() {
    wait_for_gpu_idle();
    ImGui_ImplVulkan_Shutdown();
}

void Vulkan_Rhi::imgui_new_frame() {
    ImGui_ImplVulkan_NewFrame();
}

void Vulkan_Rhi::submit_imgui_draw_data(ImDrawData *draw_data) {
    ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffers[current_frame]);
}

void Vulkan_Rhi::wait_for_gpu_idle() {
    vkDeviceWaitIdle(device);
}

void Vulkan_Rhi::begin_drawing() {
    vkWaitForFences(device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);
  
    VkResult err = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphores[current_frame], VK_NULL_HANDLE, &image_index);
  
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        begin_drawing();
    }
    else if (err != VK_SUCCESS && err != VK_SUBOPTIMAL_KHR) {
        SK_LOG_ERROR("Failed to get next swapchain image");
    }
  
    vkResetFences(device, 1, &in_flight_fences[current_frame]);
  
    vkResetCommandBuffer(command_buffers[current_frame], 0);
  
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CHECK_VK_ERR(vkBeginCommandBuffer(command_buffers[current_frame], &begin_info), "Failed to begin command buffer");
  
    VkImageMemoryBarrier image_barrier = {};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.srcAccessMask = 0;
    image_barrier.dstAccessMask = 0;
    image_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.image = swapchain_images[image_index];
    image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = 1;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;
  
    vkCmdPipelineBarrier(command_buffers[current_frame], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &image_barrier);

    VkRenderingAttachmentInfo attachment_info = {};
    attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachment_info.clearValue.color.float32[0] = 1.0f;
    attachment_info.clearValue.color.float32[1] = 0.0f;
    attachment_info.clearValue.color.float32[2] = 1.0f;
    attachment_info.clearValue.color.float32[3] = 1.0f;
    attachment_info.imageView = swapchain_image_views[image_index];
    attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo rendering_info = {};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments = &attachment_info;
    rendering_info.viewMask = 0x01;
    rendering_info.layerCount = 1;
    rendering_info.renderArea.extent = { 0, 0 };
    rendering_info.renderArea.extent = extent;
    vkCmdBeginRendering(command_buffers[current_frame], &rendering_info);
}

void Vulkan_Rhi::end_drawing() {
    vkCmdEndRendering(command_buffers[current_frame]);

    VkImageMemoryBarrier image_barrier = {};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    image_barrier.dstAccessMask = 0;
    image_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.image = swapchain_images[image_index];
    image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = 1;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;
  
    vkCmdPipelineBarrier(command_buffers[current_frame], VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, NULL, 0, NULL, 1, &image_barrier);
  
    CHECK_VK_ERR(vkEndCommandBuffer(command_buffers[current_frame]), "Failed to end command buffer");
  
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &image_available_semaphores[current_frame];
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &render_finished_semaphores[current_frame];
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers[current_frame];
    
    CHECK_VK_ERR(vkQueueSubmit(graphics_queue, 1, &submit_info, in_flight_fences[current_frame]), "Failed to submit to graphics queue");
    
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
    }
    else if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to present");
    }
    
    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}


Rhi_Command_Context *Vulkan_Rhi::get_command_context() {
    return command_context;
}
