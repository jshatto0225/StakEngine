#include "VulkanRHI.h"

#include "Log.h"
#include "Engine.h"

#include <backends/imgui_impl_vulkan.h>

#include <set>
#include <string>
#include <fstream>

#ifdef SK_GLFW
#include <glfw/glfw3.h>
#endif

struct VulkanData {
    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice gpu;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkQueue graphics_queue;
    s32 graphics_queue_index;
    u32 current_frame;
    VkCommandPool command_pool;
    VkFence in_flight_fences[MAX_FRAMES_IN_FLIGHT];
    VkDescriptorPool imgui_pool;
    Vulkan_Swapchain *active_swapchain;
};

static VulkanData vulkan_context = {};

VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
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

VkResult vulkan_create_debug_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *info, const VkAllocationCallbacks *allocator, VkDebugUtilsMessengerEXT *messenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, info, allocator, messenger);
    } else {
        SK_LOG_ERROR("Extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void vulkan_destroy_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *allocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, messenger, allocator);
    }
}

s32 vulkan_find_graphics_queue_familiy(VkPhysicalDevice gpu) {
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_family_count, queue_families.data());

    for (u32 i = 0; i < queue_family_count; i++) {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
    }

    return -1;
}

bool vulkan_get_swapchain_support(Vulkan_Swapchain_Support *out, VkSurfaceKHR surface) {
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_context.gpu, surface, &out->capabilities) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface capabilities");
        return false;
    }

    u32 format_count = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_context.gpu, surface, &format_count, nullptr) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface format count");
        return false;
    }
    out->formats.resize(format_count);

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_context.gpu, surface, &format_count, out->formats.data()) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface formats");
        return false;
    }

    u32 present_mode_count = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_context.gpu, surface, &present_mode_count, nullptr) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface present mode count");
        return false;
    }
    out->present_modes.resize(present_mode_count);

    if (vkGetPhysicalDeviceSurfacePresentModesKHR(vulkan_context.gpu, surface, &present_mode_count, out->present_modes.data()) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get surface present modes");
        return false;
    }

    return true;
}

VkAccessFlags vulkan_get_access_mask(Rhi_Resource_State state) {
    switch (state) {
    case Rhi_Resource_State::RENDER_TARGET:
        return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    case Rhi_Resource_State::PRESENT:
        return 0;
    case Rhi_Resource_State::SHADER_RESOURCE:
        return VK_ACCESS_SHADER_READ_BIT;
    default:
        return 0;
    }
}

VkImageLayout vlkan_get_image_layout(Rhi_Resource_State state) {
    switch (state) {
    case Rhi_Resource_State::RENDER_TARGET:
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case Rhi_Resource_State::PRESENT:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case Rhi_Resource_State::SHADER_RESOURCE:
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    default:
        return VK_IMAGE_LAYOUT_UNDEFINED;
    }
}

Vulkan_Queue vulkan_get_queue(Rhi_Resource_State state) {
    switch (state) {
    case Rhi_Resource_State::RENDER_TARGET:
        return Vulkan_Queue::GRAPHICS;
    case Rhi_Resource_State::PRESENT:
        return Vulkan_Queue::PRESENT;
    default:
        return Vulkan_Queue::ANY;
    }
}

VkPipelineStageFlags vulkan_get_pipeline_stage_mask(Rhi_Resource_State state) {
    switch (state) {
    case Rhi_Resource_State::RENDER_TARGET:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case Rhi_Resource_State::PRESENT:
        return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case Rhi_Resource_State::SHADER_RESOURCE:
        return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    default:
        return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
}

VkIndexType vulkan_get_index_type(Rhi_Format format) {
    switch (format) {
    case Rhi_Format::UNDEFINED:
    default:
        SK_LOG_ERROR("Invalid format for index buffer");
        return (VkIndexType) 0;
    }
}

VkDescriptorType vulkan_get_descriptor_type(Rhi_Descriptor_Type type) {
    switch (type) {
    case Rhi_Descriptor_Type::UNIFORM_BUFFER:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case Rhi_Descriptor_Type::TEXTURE:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    default:
        SK_LOG_ERROR("Invalid descriptor type");
        return (VkDescriptorType)0;
    }
}

VkShaderStageFlags vulkan_get_shader_stage_flags(std::vector<Rhi_Shader_Type> types) {
    VkShaderStageFlags flags = 0;
    for (auto type : types) {
        switch (type) {
        case Rhi_Shader_Type::VERTEX:
            flags |= VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case Rhi_Shader_Type::FRAGMENT:
            flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        }
    }

    return flags;
}

VkShaderStageFlagBits vulkan_get_shader_stage(Rhi_Shader_Type type) {
    switch (type) {
    case Rhi_Shader_Type::VERTEX:
        return VK_SHADER_STAGE_VERTEX_BIT;
    case Rhi_Shader_Type::FRAGMENT:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    default:
        SK_LOG_ERROR("Invalid shader stage");
        return(VkShaderStageFlagBits)0;
    }
}

VkVertexInputRate vulkan_get_vertex_input_rate(Rhi_Vertex_Input_Rate input_rate) {
    switch (input_rate) {
    case Rhi_Vertex_Input_Rate::PER_VERTEX:
        return VK_VERTEX_INPUT_RATE_VERTEX;
    case Rhi_Vertex_Input_Rate::PER_INSTANCE:
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    default:
        SK_LOG_ERROR("Invalid vertex input rate");
        return (VkVertexInputRate)0;
    }
}

VkFormat vulkan_get_format(Rhi_Format format) {
    switch (format) {
    case Rhi_Format::B8G8R8A8_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    default:
        return VK_FORMAT_UNDEFINED;
    }
}

VkFormat vulkan_get_depth_format(Rhi_Format format) {
    switch (format) {
    case Rhi_Format::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        return (VkFormat)0;
    }
}

Rhi_Format vulkan_get_rhi_format(VkFormat format) {
    switch (format) {
    case VK_FORMAT_B8G8R8A8_SRGB:
        return Rhi_Format::B8G8R8A8_SRGB;
    default:
        SK_LOG_ERROR("Invalid vulkan format");
        return Rhi_Format::UNDEFINED;
    }
}

VkFormat vulkan_get_stencil_format(Rhi_Format format) {
    switch (format) {
    case Rhi_Format::UNDEFINED:
        return VK_FORMAT_UNDEFINED;
    default:
        SK_LOG_ERROR("Invalid stencil format");
        return (VkFormat)0;
    }
}

s32 find_present_queue_index(VkSurfaceKHR surface) {
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_context.gpu, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(vulkan_context.gpu, &queue_family_count, queue_families.data());

    for (u32 i = 0; i < queue_family_count; i++) {
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(vulkan_context.gpu, i, surface, &present_support);
        if (present_support) {
            return i;
        }
    }

    return -1;
}

u32 vulkan_find_memory_type(u32 filter, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(vulkan_context.gpu, &mem_props);

    for (u32 i = 0; i < mem_props.memoryTypeCount; i++) {
        if ((filter & (1 << i)) && ((mem_props.memoryTypes[i].propertyFlags & flags) == flags)) {
            return i;
        }
    }

    SK_LOG_ERROR("Failed to get memory type");
    return 0;
}
VkCommandBuffer vulkan_begin_one_time_command_buffer() {
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = vulkan_context.command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    if (vkAllocateCommandBuffers(vulkan_context.device, &alloc_info, &command_buffer) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate one time command buffer");
        return nullptr;
    }

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to begin one time command buffer");
        vkFreeCommandBuffers(vulkan_context.device, vulkan_context.command_pool, 1, &command_buffer);
        return nullptr;
    }

    return command_buffer;
}

bool vulkan_end_one_time_command_buffer(VkCommandBuffer command_buffer) {
    if (command_buffer == nullptr) {
        SK_LOG_ERROR("Invalid command buffer provided");
        return false;
    }

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    if (vkQueueSubmit(vulkan_context.graphics_queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit one time command buffer");
        vkFreeCommandBuffers(vulkan_context.device, vulkan_context.command_pool, 1, &command_buffer);
        return false;
    }
    if (vkQueueWaitIdle(vulkan_context.graphics_queue) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to wait for queue");
        vkFreeCommandBuffers(vulkan_context.device, vulkan_context.command_pool, 1, &command_buffer);
        return false;
    }

    vkFreeCommandBuffers(vulkan_context.device, vulkan_context.command_pool, 1, &command_buffer);

    return true;
}

bool vulkan_copy_buffer(VkBuffer dst, VkBuffer src, u32 size) {
    VkCommandBuffer command_buffer = vulkan_begin_one_time_command_buffer();

    if (command_buffer == nullptr) {
        SK_LOG_ERROR("Failed to get command buffer for buffer copy");
        return false;
    }

    VkBufferCopy copy_region = {};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;

    vkCmdCopyBuffer(command_buffer, dst, src, 1, &copy_region);

    if (!vulkan_end_one_time_command_buffer(command_buffer)) {
        SK_LOG_ERROR("Failed to submit command buffer for buffer copy");
        return false;
    }

    return true;
}

bool vulkan_create_buffer(u32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *memory) {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(vulkan_context.device, &buffer_info, nullptr, buffer) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create buffer");
        return false;
    }

    VkMemoryRequirements mem_reqs = {};
    vkGetBufferMemoryRequirements(vulkan_context.device, *buffer, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = vulkan_find_memory_type(mem_reqs.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkan_context.device, &alloc_info, nullptr, memory) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate buffer memory");
        return false;
    }

    if (vkBindBufferMemory(vulkan_context.device, *buffer, *memory, 0) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to bind buffer memory");
        return false;
    }

    return true;
}

bool vulkan_create_swapchain_texture(Vulkan_Texture *out, VkImage image, VkExtent2D extent, VkFormat format) {
    out->swapchain_texture = true;
    out->extent = extent;
    out->format = format;
    out->image = image;
    out->render_area = { 1, 0, 0, extent.width, extent.height };

    if (!vulkan_create_image_view(&out->image_view, image, out->format)) {
        SK_LOG_ERROR("Failed to create image view");
        return false;
    }

    out->subresource_ranges = (VkImageSubresourceRange *)malloc(sizeof(VkImageSubresourceRange));
    out->subresource_range_count = 1;
    out->subresource_ranges[0] = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    return true;
}

bool vulkan_create_image_view(VkImageView *out, VkImage image, VkFormat format) {
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

    if (vkCreateImageView(vulkan_context.device, &view_info, nullptr, out) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image view");
        return false;
    }

    return true;
}

bool vulkan_create_image(u32 width, u32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags flags, VkMemoryPropertyFlags properties, VkImage *out_image, VkDeviceMemory *out_image_memory) {
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = flags;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(vulkan_context.device, &image_info, nullptr, out_image) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image");
        return false;
    }

    VkMemoryRequirements mem_reqs = {};
    vkGetImageMemoryRequirements(vulkan_context.device, *out_image, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = vulkan_find_memory_type(mem_reqs.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkan_context.device, &alloc_info, nullptr, out_image_memory) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate image memory");
        return false;
    }

    vkBindImageMemory(vulkan_context.device, *out_image, *out_image_memory, 0);

    return true;
}

bool vulkan_create_texture_sampler(VkSampler *out) {
    VkSamplerCreateInfo sampler_info = {};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties = { 0 };
    vkGetPhysicalDeviceProperties(vulkan_context.gpu, &properties);
    sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    if (vkCreateSampler(vulkan_context.device, &sampler_info, nullptr, out) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create image sampler");
        return false;
    }

    return true;
}

void vulkan_transition_barrier(Vulkan_Command_List *list, Rhi_Transition_Barrier *barrier) {
    switch (rhi_get_transition_type(barrier->state_before, barrier->state_after)) {
    case Rhi_Transition_Type::IMAGE: {
        auto texture = (Vulkan_Texture *)barrier->resource;

        VkImageMemoryBarrier image_barrier = {};
        image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_barrier.srcAccessMask = vulkan_get_access_mask(barrier->state_before);
        image_barrier.dstAccessMask = vulkan_get_access_mask(barrier->state_after);
        image_barrier.oldLayout = vlkan_get_image_layout(barrier->state_before);
        image_barrier.newLayout = vlkan_get_image_layout(barrier->state_after);
        Vulkan_Queue SrcQueue = vulkan_get_queue(barrier->state_after);
        if (SrcQueue == Vulkan_Queue::ANY) {
            image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        } else if (SrcQueue == Vulkan_Queue::GRAPHICS) {
            image_barrier.srcQueueFamilyIndex = vulkan_context.graphics_queue_index;
        } else if (SrcQueue == Vulkan_Queue::PRESENT) {
            image_barrier.srcQueueFamilyIndex = vulkan_context.active_swapchain->present_queue_index;
        } else {
            SK_LOG_ERROR("Unsupported source queue");
            return;
        }
        Vulkan_Queue dst_queue = vulkan_get_queue(barrier->state_after);
        if (dst_queue == Vulkan_Queue::ANY) {
            image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        } else if (dst_queue == Vulkan_Queue::GRAPHICS) {
            image_barrier.dstQueueFamilyIndex = vulkan_context.graphics_queue_index;
        } else if (dst_queue == Vulkan_Queue::PRESENT) {
            image_barrier.dstQueueFamilyIndex = vulkan_context.active_swapchain->present_queue_index;
        } else {
            SK_LOG_ERROR("Unsupported destination queue");
            return;
        }

        image_barrier.image = texture->image;
        image_barrier.subresourceRange = texture->subresource_ranges[barrier->subresource];

        // TODO: Batch pipeline barrier calls
        vkCmdPipelineBarrier(list->buffers[vulkan_context.current_frame], vulkan_get_pipeline_stage_mask(barrier->state_before), vulkan_get_pipeline_stage_mask(barrier->state_after), 0, 0, nullptr, 0, nullptr, 1, &image_barrier);
        break;
    }
    default:
        SK_LOG_WARN("Unsupported resource transition type");
        return;
    }
}

// Rhi funcions

Rhi vulkan_create_rhi() {
    return {
        vulkan_init,
        vulkan_shutdown,
        vulkan_imgui_new_frame,
        vulkan_shutdown_imgui,
        vulkan_init_imgui,
        vulkan_wait_for_gpu_idle,
        vulkan_prepare_frame,
        vulkan_present_frame,
        vulkan_submit,
        vulkan_add_texture_to_imgui_window,
        vulkan_create_texture,
        vulkan_destroy_texture,
        vulkan_create_swapchain,
        vulkan_destroy_swapchain,
        vulkan_create_command_list,
        vulkan_destroy_command_list,
        vulkan_create_shader,
        vulkan_destroy_shader,
        vulkan_create_vertex_buffer,
        vulkan_create_index_buffer,
        vulkan_destroy_buffer,
        VulkanCreatePipelineLayout,
        VulkanDestroyPipelineLayout,
        vulkan_create_descriptor_set_layout,
        vulkan_destroy_descriptor_set_layout,
        vulkan_create_graphics_pipeline,
        vulkan_destroy_pipeline,
        vulkan_set_buffer_data,
        vulkan_map_buffer_data,
        vulkan_send_buffer_to_gpu,
        vulkan_get_current_swapchain_texture,
        vulkan_get_texture_format,
        vulkan_get_texture_render_area,
        vulkan_begin_command_list,
        vulkan_end_command_list,
        vulkan_cmd_render_imgui_draw_data,
        vulkan_cmd_resource_barrier,
        vulkan_cmd_set_render_target,
        vulkan_cmd_unset_render_target,
        vulkan_cmd_bind_vertex_buffer,
        vulkan_cmd_bind_index_buffer,
        vulkan_cmd_draw_indexed,
        vulkan_cmd_draw_instanced,
        vulkan_cmd_set_viewport,
        vulkan_cmd_set_scissor,
        vulkan_cmd_bind_pipeline,
        vulkan_notify_swapchain_of_resize,
    };
}

bool vulkan_init() {
    bool extensions_supported = true;
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> AvailableLayers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, AvailableLayers.data());

    for (auto &validation_layer : VALIDATION_LAYERS) {
        bool layer_found = false;

        for (auto &available_layer : AvailableLayers) {
            if (strcmp(validation_layer, available_layer.layerName) != 0) {
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
        return false;
    }

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "App Name"; // TODO:
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "StakEngine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_3;

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
        debug_messenger_info.pfnUserCallback = vulkan_messenger;

        instance_info.pNext = &debug_messenger_info;
    }

#ifdef SK_WINDOWS
    std::vector<const char *> extensions = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
    };
#else
    std::vector<const char *> extensions;
#endif

    if (ENABLE_VALIDATION_LAYERS) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instance_info.enabledExtensionCount = (u32)extensions.size();
    instance_info.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&instance_info, nullptr, &vulkan_context.instance) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan instance");
        return false;
    }
    if (vulkan_create_debug_messenger(vulkan_context.instance, &debug_messenger_info, nullptr, &vulkan_context.debug_messenger) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan debug messenger");
        return false;
    }

    u32 device_count = 0;
    vkEnumeratePhysicalDevices(vulkan_context.instance, &device_count, nullptr);
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(vulkan_context.instance, &device_count, devices.data());

    bool device_found = false;
    for (u32 i = 0; i < device_count; i++) {
        bool is_device_suitable = false;
        vulkan_context.graphics_queue_index = vulkan_find_graphics_queue_familiy(devices[i]);

        u32 device_extension_count = 0;
        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &device_extension_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(device_extension_count);

        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &device_extension_count, available_extensions.data());

        bool extensions_supported = false;

        for (u32 j = 0; j < device_extension_count; j++) {
            if (std::strcmp(available_extensions[j].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) != 0) {
                extensions_supported = true;
                break;
            }
        }

        VkPhysicalDeviceFeatures supported_features;
        vkGetPhysicalDeviceFeatures(devices[i], &supported_features);

        is_device_suitable = extensions_supported && vulkan_context.graphics_queue_index >= 0 && supported_features.samplerAnisotropy;

        if (is_device_suitable) {
            vulkan_context.gpu = devices[i];
            device_found = true;
            break;
        }
    }

    if (!device_found) {
        SK_LOG_ERROR("Failed to find suitable physical device");
        return false;
    }

    f32 queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueFamilyIndex = vulkan_context.graphics_queue_index;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = &queue_priority;

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

    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.pEnabledFeatures = &device_features;
    device_info.enabledExtensionCount = (u32)DEVICE_EXTENSIONS.size();
    device_info.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

    if (ENABLE_VALIDATION_LAYERS) {
        device_info.enabledLayerCount = (u32)VALIDATION_LAYERS.size();
        device_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    if (vkCreateDevice(vulkan_context.gpu, &device_info, nullptr, &vulkan_context.device) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan device");
        return false;
    }

    vkGetDeviceQueue(vulkan_context.device, vulkan_context.graphics_queue_index, 0, &vulkan_context.graphics_queue);

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto &fence : vulkan_context.in_flight_fences) {
        if (vkCreateFence(vulkan_context.device, &fence_info, nullptr, &fence) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan fence");
            return false;
        }
    }

    VkCommandPoolCreateInfo command_pool_info = {};

    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_info.queueFamilyIndex = vulkan_context.graphics_queue_index;

    if (vkCreateCommandPool(vulkan_context.device, &command_pool_info, nullptr, &vulkan_context.command_pool) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan command pool");
        return false;
    }

    return true;
}

void vulkan_shutdown() {
    for (auto &fence : vulkan_context.in_flight_fences) {
        vkDestroyFence(vulkan_context.device, fence, nullptr);
    }

    vkDestroyCommandPool(vulkan_context.device, vulkan_context.command_pool, nullptr);

    vkDestroyDevice(vulkan_context.device, nullptr);

    vulkan_destroy_debug_messenger(vulkan_context.instance, vulkan_context.debug_messenger, nullptr);

    vkDestroyInstance(vulkan_context.instance, nullptr);
}

bool vulkan_recreate_swapchain(Vulkan_Swapchain *swapchain) {
    swapchain->image_index = 0;

#ifdef SK_GLFW
    s32 width = 0;
    s32 height = 0;
    auto window = (GLFWwindow *)swapchain->window_handle;

    glfwGetFramebufferSize(window, &width, &height);

    while (width == 0 || height == 0) {
        glfwPollEvents();
        glfwGetFramebufferSize(window, &width, &height);
    }
#endif

    if (!rhi.wait_for_gpu_idle()) {
        SK_LOG_ERROR("Failed to wait for gpu before swapchain recreate");
        return false;
    }

    for (u32 i = 0; i < swapchain->image_count; i++)
        vkDestroyImageView(vulkan_context.device, swapchain->images[i].image_view, nullptr);

    vkDestroySwapchainKHR(vulkan_context.device, swapchain->swapchain, nullptr);

    Vulkan_Swapchain_Support support;
    if (!vulkan_get_swapchain_support(&support, swapchain->surface)) {
        SK_LOG_ERROR("Failed to get swapchain support");
        return false;
    }

    VkSurfaceFormatKHR format = {};
    for (auto &f : support.formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = f;
            break;
        }
    }

    VkPresentModeKHR present_mode = {};
    for (auto &Mode : support.present_modes) {
        if (Mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = Mode;
            break;
        }
    }

    VkExtent2D swapchain_extent = {};

    if (support.capabilities.currentExtent.width != UINT32_MAX) {
        swapchain_extent = support.capabilities.currentExtent;
    } else {
#ifdef SK_GLFW
        s32 width;
        s32 height;
        glfwGetFramebufferSize(window, &width, &height);
#endif

        VkExtent2D extent = {
            (u32)width,
            (u32)height
        };

        extent.width = std::clamp(extent.width, support.capabilities.minImageExtent.width, support.capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height);

        swapchain_extent = extent;
    }

    u32 image_count = support.capabilities.minImageCount + 1;

    if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount) {
        image_count = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = swapchain->surface;
    swapchain_info.minImageCount = swapchain->image_count;
    swapchain_info.imageFormat = format.format;
    swapchain_info.imageColorSpace = format.colorSpace;
    swapchain_info.imageExtent = swapchain_extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapchain->present_queue_index = find_present_queue_index(swapchain->surface);

    assert(vulkan_context.graphics_queue_index == swapchain->present_queue_index);

    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_info.preTransform = support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(vulkan_context.device, &swapchain_info, nullptr, &swapchain->swapchain) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create swapchian");
        return false;
    }

    VkImage *images = (VkImage *)malloc(sizeof(VkImage) * image_count);
    vkGetSwapchainImagesKHR(vulkan_context.device, swapchain->swapchain, &image_count, images);

    if (swapchain->image_count != image_count)
        swapchain->images = (Vulkan_Texture *)realloc(swapchain->images, sizeof(Vulkan_Texture) * image_count);

    for (u32 i = 0; i < swapchain->image_count; i++) {
        if (!vulkan_create_swapchain_texture(&swapchain->images[i], images[i], swapchain_extent, format.format)) {
            SK_LOG_ERROR("Failed to create backbuffer");
            return false;
        }
    }

    swapchain->framebuffer_resized = false;
    swapchain->image_count = image_count;
    swapchain->min_image_count = image_count;

    return true;
}

void vulkan_imgui_new_frame() {
    ImGui_ImplVulkan_NewFrame();
}

void vulkan_shutdown_imgui() {
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(vulkan_context.device, vulkan_context.imgui_pool, nullptr);
}

void vulkan_init_imgui(Rhi_Resource_Handle swapchain) {
    auto VulkanSwapchain = (Vulkan_Swapchain *)swapchain;
    VkFormat formats[] = { VulkanSwapchain->format };

    VkPipelineRenderingCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipeline_info.viewMask = 0x01;
    pipeline_info.colorAttachmentCount = 1;
    pipeline_info.pColorAttachmentFormats = formats;
    pipeline_info.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    pipeline_info.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.ApiVersion = VK_API_VERSION_1_3;
    init_info.Instance = vulkan_context.instance;
    init_info.PhysicalDevice = vulkan_context.gpu;
    init_info.Device = vulkan_context.device;
    init_info.QueueFamily = vulkan_context.graphics_queue_index;
    init_info.Queue = vulkan_context.graphics_queue;
    init_info.MinImageCount = VulkanSwapchain->min_image_count;
    init_info.ImageCount = VulkanSwapchain->image_count;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.DescriptorPoolSize = 1000;
    init_info.UseDynamicRendering = true;
    init_info.PipelineRenderingCreateInfo = pipeline_info;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = [](VkResult Err) {
        if (Err != VK_SUCCESS)
            SK_LOG_ERROR("ImGui Vulkan Error");
    };
    init_info.MinAllocationSize = 1024 * 1024;

    ImGui_ImplVulkan_Init(&init_info);
}

bool vulkan_wait_for_gpu_idle() {
    if (vkDeviceWaitIdle(vulkan_context.device) != VK_SUCCESS) 
        return false;
    return true;
}

bool vulkan_prepare_frame(Rhi_Resource_Handle swapchain) {
    if (vulkan_context.active_swapchain) {
        SK_LOG_ERROR("Swapchain already set");
        return false;
    }

    auto vulkan_swapchain = (Vulkan_Swapchain *)swapchain;
    if (vkAcquireNextImageKHR(vulkan_context.device, vulkan_swapchain->swapchain, UINT64_MAX, vulkan_swapchain->image_available_semaphores[vulkan_context.current_frame], nullptr, &vulkan_swapchain->image_index) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to get next swapchain image");
        return false;
    }

    vulkan_context.active_swapchain = vulkan_swapchain;

    return true;
}

bool vulkan_present_frame() {
    if (vulkan_context.active_swapchain == nullptr) {
        SK_LOG_WARN("No swapchain set for rendering");
        return false;
    }

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &vulkan_context.active_swapchain->swapchain;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &vulkan_context.active_swapchain->render_finished_semaphores[vulkan_context.current_frame];
    present_info.pImageIndices = &vulkan_context.active_swapchain->image_index;
    VkResult err = vkQueuePresentKHR(vulkan_context.active_swapchain->present_queue, &present_info);

    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR || vulkan_context.active_swapchain->framebuffer_resized) {
        if (!vulkan_recreate_swapchain(vulkan_context.active_swapchain)) {
            SK_LOG_ERROR("Failed to recreate swapchian");
            return false;
        }
    } else if (err != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to present");
        return false;
    }

    vulkan_context.current_frame = (vulkan_context.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    if (vkWaitForFences(vulkan_context.device, 1, &vulkan_context.in_flight_fences[vulkan_context.current_frame], VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to wait for fences");
        return false;
    }

    vulkan_context.active_swapchain = nullptr;

    return true;
}

bool vulkan_submit(Rhi_Resource_Handle list) {
    if (vulkan_context.active_swapchain == nullptr) {
        SK_LOG_WARN("No swapchain set for rendering");
        return false;
    }

    auto command_list = (Vulkan_Command_List *)list;
    VkCommandBuffer CommandBuffer = command_list->buffers[vulkan_context.current_frame];

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &vulkan_context.active_swapchain->image_available_semaphores[vulkan_context.current_frame];
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &vulkan_context.active_swapchain->render_finished_semaphores[vulkan_context.current_frame];
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &CommandBuffer;

    if (vkResetFences(vulkan_context.device, 1, &vulkan_context.in_flight_fences[vulkan_context.current_frame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to reset fencecs");
        return false;
    }

    if (vkQueueSubmit(vulkan_context.graphics_queue, 1, &submit_info, vulkan_context.in_flight_fences[vulkan_context.current_frame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to submit to graphics queue");
        return false;
    }

    return true;
}

void vulkan_add_texture_to_imgui_window(Rhi_Resource_Handle texture) {
    auto vulkan_texture = (Vulkan_Texture *)texture;

    ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();

    // Keep aspect ratio
    f32 aspect = (f32)vulkan_texture->extent.width / (f32)vulkan_texture->extent.height;
    ImVec2 image_size;
    if (viewport_panel_size.x / aspect <= viewport_panel_size.y) {
        image_size.x = viewport_panel_size.x;
        image_size.y = viewport_panel_size.x / aspect;
    } else {
        image_size.y = viewport_panel_size.y;
        image_size.x = viewport_panel_size.y * aspect;
    }

    // Center Image
    ImVec2 offset = { (viewport_panel_size.x - image_size.x) * 0.5f, (viewport_panel_size.y - image_size.y) * 0.5f };
    offset.x = (offset.x > 0) ? offset.x : 0;
    offset.y = (offset.y > 0) ? offset.y : 0;
    ImVec2 CursorPos = ImGui::GetCursorPos();
    CursorPos.x += offset.x;
    CursorPos.y += offset.y;
    ImGui::SetCursorPos(CursorPos);

    ImGui::Image((u64)vulkan_texture->imgui_descriptor_set, image_size);
}

Rhi_Resource_Handle vulkan_create_texture(Rhi_Texture_Description *description) {
    auto texture = (Vulkan_Texture *)malloc(sizeof(Vulkan_Texture));

    texture->swapchain_texture = false;
    texture->format = vulkan_get_format(description->format);
    texture->extent = { description->width, description->height };
    texture->render_area = { 1, 0, 0, description->width, description->height };

    texture->is_shader_resource = description->use_as_shader_resource;
    texture->is_imgui_compatible = description->use_for_imgui;

    if (!vulkan_create_image(texture->extent.width, texture->extent.height, texture->format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->image, &texture->image_memory)) {
        SK_LOG_ERROR("Failed to create offscreen render target image");
        return 0;
    }

    if (!vulkan_create_image_view(&texture->image_view, texture->image, texture->format)) {
        SK_LOG_ERROR("Failed to create image view for offscreen buffer");
        return 0;
    }

    if (texture->is_shader_resource) {
        if (!vulkan_create_texture_sampler(&texture->sampler)) {
            SK_LOG_ERROR("Failed to create sampler for offscreen backbuffer");
            return 0;
        }
    } else {
        texture->sampler = nullptr;
    }

    if (texture->is_imgui_compatible) {
        texture->imgui_descriptor_set = ImGui_ImplVulkan_AddTexture(texture->sampler, texture->image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        if (!texture->imgui_descriptor_set) {
            SK_LOG_ERROR("Failed to create imgui descriptor set");
            return 0;
        }
    } else {
        texture->imgui_descriptor_set = nullptr;
    }

    texture->subresource_ranges = (VkImageSubresourceRange *)malloc(sizeof(VkImageSubresourceRange));
    texture->subresource_range_count = 1;
    texture->subresource_ranges[0] = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    return (Rhi_Resource_Handle)texture;
}

void vulkan_destroy_texture(Rhi_Resource_Handle *resource) {
    auto texture = (Vulkan_Texture *)*resource;

    if (!texture->swapchain_texture) {
        if (texture->is_imgui_compatible)
            ImGui_ImplVulkan_RemoveTexture(texture->imgui_descriptor_set);
    }

    vkDestroyImageView(vulkan_context.device, texture->image_view, nullptr);

    if (!texture->swapchain_texture) {
        vkFreeMemory(vulkan_context.device, texture->image_memory, nullptr);
        vkDestroyImage(vulkan_context.device, texture->image, nullptr);
        if (texture->is_shader_resource)
            vkDestroySampler(vulkan_context.device, texture->sampler, nullptr);
    }

    free(texture);

    *resource = 0;
}

Rhi_Resource_Handle vulkan_create_swapchain(Window *window) {
    auto swapchain = (Vulkan_Swapchain *)malloc(sizeof(Vulkan_Swapchain));

    swapchain->window_handle = window->platform_handle;

#ifdef SK_GLFW
    if (glfwCreateWindowSurface(vulkan_context.instance, (GLFWwindow *)window->platform_handle, nullptr, &swapchain->surface) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create window surface");
        return 0;
    }
#endif

    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (u32 Index = 0; Index < MAX_FRAMES_IN_FLIGHT; Index++) {
        if (vkCreateSemaphore(vulkan_context.device, &semaphore_info, nullptr, &swapchain->image_available_semaphores[Index]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            return 0;
        }
        if (vkCreateSemaphore(vulkan_context.device, &semaphore_info, nullptr, &swapchain->render_finished_semaphores[Index]) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to create vulkan semaphore");
            return 0;
        }
    }

    Vulkan_Swapchain_Support support;
    if (!vulkan_get_swapchain_support(&support, swapchain->surface)) {
        SK_LOG_ERROR("Failed to get swapchain support");
        return 0;
    }

    VkSurfaceFormatKHR format = {};
    for (auto &f : support.formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = f;
            break;
        }
    }

    VkPresentModeKHR present_mode = {};
    for (auto &Mode : support.present_modes) {
        if (Mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = Mode;
            break;
        }
    }

    VkExtent2D swapchain_extent = {};

    if (support.capabilities.currentExtent.width != UINT32_MAX) {
        swapchain_extent = support.capabilities.currentExtent;
    } else {
#ifdef SK_GLFW
        s32 width;
        s32 height;
        glfwGetFramebufferSize((GLFWwindow *)window, &width, &height);
#endif

        VkExtent2D extent = {
            static_cast<u32>(width),
            static_cast<u32>(height)
        };

        extent.width = std::clamp(extent.width, support.capabilities.minImageExtent.width, support.capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, support.capabilities.minImageExtent.height, support.capabilities.maxImageExtent.height);

        swapchain_extent = extent;
    }

    swapchain->image_count = support.capabilities.minImageCount + 1;

    if (support.capabilities.maxImageCount > 0 && swapchain->image_count > support.capabilities.maxImageCount) {
        swapchain->image_count = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = swapchain->surface;
    swapchain_info.minImageCount = swapchain->image_count;
    swapchain_info.imageFormat = format.format;
    swapchain_info.imageColorSpace = format.colorSpace;
    swapchain_info.imageExtent = swapchain_extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapchain->min_image_count = swapchain->image_count;

    swapchain->present_queue_index = find_present_queue_index(swapchain->surface);

    assert(vulkan_context.graphics_queue_index == swapchain->present_queue_index);

    swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_info.preTransform = support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(vulkan_context.device, &swapchain_info, nullptr, &swapchain->swapchain) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create swapchian");
        return 0;
    }

    std::vector<VkImage> swapchain_images = {};
    swapchain_images.resize(swapchain->image_count);

    vkGetSwapchainImagesKHR(vulkan_context.device, swapchain->swapchain, &swapchain->image_count, swapchain_images.data());

    swapchain->images = (Vulkan_Texture *)malloc(swapchain->image_count * sizeof(Vulkan_Texture));
    for (u32 i = 0; i < swapchain->image_count; i++) {
        if (!vulkan_create_swapchain_texture(&swapchain->images[i], swapchain_images[i], swapchain_extent, format.format)) {
            SK_LOG_ERROR("Failed to create swapchain texture");
            return 0;
        }
    }

    vkGetDeviceQueue(vulkan_context.device, swapchain->present_queue_index, 0, &swapchain->present_queue);

    swapchain->image_index = 0;
    swapchain->format = format.format;

    return (Rhi_Resource_Handle)swapchain;
}

void vulkan_destroy_swapchain(Rhi_Resource_Handle *resource) {
    auto swapchain = (Vulkan_Swapchain *)*resource;
    for (u32 i = 0; i < swapchain->image_count; i++)
        vkDestroyImageView(vulkan_context.device, swapchain->images[i].image_view, nullptr);

    vkDestroySwapchainKHR(vulkan_context.device, swapchain->swapchain, nullptr);

    vkDestroySurfaceKHR(vulkan_context.instance, swapchain->surface, nullptr);

    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vulkan_context.device, swapchain->image_available_semaphores[i], nullptr);
        vkDestroySemaphore(vulkan_context.device, swapchain->render_finished_semaphores[i], nullptr);
    }

    free(swapchain);
    *resource = 0;
}

Rhi_Resource_Handle vulkan_create_command_list(Rhi_Command_List_Description *description) {
    auto command_list = (Vulkan_Command_List *)malloc(sizeof(Vulkan_Command_List));

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    info.commandPool = vulkan_context.command_pool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    if (description->is_secondary) {
        info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    }

    if (vkAllocateCommandBuffers(vulkan_context.device, &info, command_list->buffers) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to allocate command buffers");
        return 0;
    }

    return (Rhi_Resource_Handle)command_list;
}

void vulkan_destroy_command_list(Rhi_Resource_Handle *resource) {
    auto command_list = (Vulkan_Command_List *)*resource;
    vkFreeCommandBuffers(vulkan_context.device, vulkan_context.command_pool, MAX_FRAMES_IN_FLIGHT, command_list->buffers);
    free(command_list);
    *resource = 0;
}

Rhi_Resource_Handle vulkan_create_shader(Rhi_Shader_Description *description) {
    auto shader = (Vulkan_Shader *)malloc(sizeof(Vulkan_Shader));

    shader->state = vulkan_get_shader_stage(description->type);

    std::string path = "Assets/Shaders/";
    path.append(description->name);
    path.append(".spv");
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        SK_LOG_ERROR("Failed to open shader file");
        return 0;
    }

    u32 file_size = (u32)file.tellg();
    std::vector<char> ShaderCode(file_size);
    file.seekg(0);
    file.read(ShaderCode.data(), file_size);
    file.close();

    VkShaderModuleCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = (u32)ShaderCode.size();
    info.pCode = (const u32 *)ShaderCode.data();

    if (vkCreateShaderModule(vulkan_context.device, &info, nullptr, &shader->shader) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create shader module");
        return 0;
    }

    return (Rhi_Resource_Handle)shader;
}

void vulkan_destroy_shader(Rhi_Resource_Handle *resource) {
    auto shader = (Vulkan_Shader *)*resource;
    vkDestroyShaderModule(vulkan_context.device, shader->shader, nullptr);
    free(shader);
    *resource = 0;
}

Rhi_Resource_Handle vulkan_create_vertex_buffer(Rhi_Vertex_Buffer_Description *description) {
    auto buffer = (Vulkan_Buffer *)malloc(sizeof(Vulkan_Buffer));

    buffer->use_staging_buffer = description->use_staging_buffer;
    buffer->element_count = description->element_count;
    buffer->size = description->element_count * description->layout.stride;
    buffer->type = Rhi_Buffer_Type::VERTEX;
    buffer->layout = description->layout;

    if (buffer->use_staging_buffer) {
        if (!vulkan_create_buffer(buffer->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer->staging_buffer, &buffer->staging_buffer_memory)) {
            SK_LOG_ERROR("Failed to create staging buffer");
            return 0;
        }

        if (!vulkan_create_buffer(buffer->size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer->buffer, &buffer->buffer_memory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(vulkan_context.device, buffer->staging_buffer_memory, 0, buffer->size, 0, &buffer->mapped_data) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    } else {
        if (!vulkan_create_buffer(buffer->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer->buffer, &buffer->buffer_memory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(vulkan_context.device, buffer->buffer_memory, 0, buffer->size, 0, &buffer->mapped_data) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    }

    if (description->initial_contents != nullptr) {
        if (description->initial_contents_size > buffer->size) {
            SK_LOG_WARN("Initial contents size is greater thatn buffer size");
            return (Rhi_Resource_Handle)buffer;
        }

        memcpy(buffer->mapped_data, description->initial_contents, description->initial_contents_size);
        if (!vulkan_send_buffer_to_gpu((Rhi_Resource_Handle)buffer)) {
            SK_LOG_ERROR("Failed to send buffer to gpu");
            return 0;
        }
    }

    return (Rhi_Resource_Handle)buffer;
}

Rhi_Resource_Handle vulkan_create_index_buffer(Rhi_Index_Buffer_Description *description) {
    auto buffer = (Vulkan_Buffer *) malloc(sizeof(Vulkan_Buffer));

    buffer->use_staging_buffer = description->use_staging_buffer;
    buffer->element_count = description->count;
    buffer->size = description->count * sizeof(u16);
    buffer->type = Rhi_Buffer_Type::INDEX;
    buffer->layout = {};

    if (buffer->use_staging_buffer) {
        if (!vulkan_create_buffer(buffer->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer->staging_buffer, &buffer->staging_buffer_memory)) {
            SK_LOG_ERROR("Failed to create staging buffer");
            return 0;
        }

        if (!vulkan_create_buffer(buffer->size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &buffer->buffer, &buffer->buffer_memory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(vulkan_context.device, buffer->staging_buffer_memory, 0, buffer->size, 0, &buffer->mapped_data) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    } else {
        if (!vulkan_create_buffer(buffer->size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &buffer->buffer, &buffer->buffer_memory)) {
            SK_LOG_ERROR("Failed to create device buffer");
            return 0;
        }

        if (vkMapMemory(vulkan_context.device, buffer->buffer_memory, 0, buffer->size, 0, &buffer->mapped_data) != VK_SUCCESS) {
            SK_LOG_ERROR("Failed to map buffer memory");
            return 0;
        }
    }

    if (description->initial_contents != nullptr) {
        if (description->initial_contents_size > buffer->size) {
            SK_LOG_WARN("Initial contents size is greater thatn buffer size");
            return (Rhi_Resource_Handle)buffer;
        }

        memcpy(buffer->mapped_data, description->initial_contents, description->initial_contents_size);
        if (!vulkan_send_buffer_to_gpu((Rhi_Resource_Handle)buffer)) {
            SK_LOG_ERROR("Failed to send buffer to gpu");
            return 0;
        }
    }

    return (Rhi_Resource_Handle)buffer;
}

void vulkan_destroy_buffer(Rhi_Resource_Handle *resource) {
    auto buffer = (Vulkan_Buffer *)*resource;

    if (buffer->use_staging_buffer) {
        vkFreeMemory(vulkan_context.device, buffer->staging_buffer_memory, nullptr);
        vkDestroyBuffer(vulkan_context.device, buffer->staging_buffer, nullptr);
    }

    vkFreeMemory(vulkan_context.device, buffer->buffer_memory, nullptr);
    vkDestroyBuffer(vulkan_context.device, buffer->buffer, nullptr);

    free(buffer);
    *resource = 0;
}

Rhi_Resource_Handle VulkanCreatePipelineLayout(Rhi_Pipeline_Layout_Description *description) {
    auto layout = (Vulkan_Pipeline_Layout *)malloc(sizeof(Vulkan_Pipeline_Layout));

    VkPipelineLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayout> layouts = {};
    layouts.reserve(description->descriptor_set_layout_count * sizeof(VkDescriptorSetLayout));

    for (u32 i = 0; i < description->descriptor_set_layout_count; i++) {
        auto layout = (Vulkan_Descriptor_Set_Layout *)description->descriptor_set_layouts[i];
        layouts.push_back(layout->layout);
    }

    info.setLayoutCount = (u32)layouts.size();
    info.pSetLayouts = layouts.data();

    if (vkCreatePipelineLayout(vulkan_context.device, &info, nullptr, &layout->layout) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create pipeline layout");
        return 0;
    }

    return (Rhi_Resource_Handle)layout;
}

void VulkanDestroyPipelineLayout(Rhi_Resource_Handle *resource) {
    auto layout = (Vulkan_Pipeline_Layout *)*resource;
    vkDestroyPipelineLayout(vulkan_context.device, layout->layout, nullptr);
    free(layout);
    *resource = 0;
}

Rhi_Resource_Handle vulkan_create_descriptor_set_layout(Rhi_Descriptor_Set_Layout_Description *description) {
    auto layout = (Vulkan_Descriptor_Set_Layout *)malloc(sizeof(Vulkan_Descriptor_Set_Layout));

    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    std::vector<VkDescriptorSetLayoutBinding> Bindings;

    for (u32 i = 0; i < description->descriptor_count; i++) {
        auto descriptor = description->descriptors[i];
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = descriptor.binding;
        binding.descriptorType = vulkan_get_descriptor_type(descriptor.type);
        binding.descriptorCount = descriptor.count;
        for (u32 j = 0; j < descriptor.shader_stage_count; j++)
            binding.stageFlags |= vulkan_get_shader_stage(descriptor.stages[j]);

        Bindings.push_back(binding);
    }

    info.bindingCount = (u32)Bindings.size();
    info.pBindings = Bindings.data();

    if (vkCreateDescriptorSetLayout(vulkan_context.device, &info, nullptr, &layout->layout) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create descriptor set layout");
        return 0;
    }

    return (Rhi_Resource_Handle)layout;
}

void vulkan_destroy_descriptor_set_layout(Rhi_Resource_Handle *resource) {
    auto layout = (Vulkan_Descriptor_Set_Layout *)*resource;
    vkDestroyDescriptorSetLayout(vulkan_context.device, layout->layout, nullptr);
    free(layout);
    *resource = 0;
}

Rhi_Resource_Handle vulkan_create_graphics_pipeline(Rhi_Graphics_Pipeline_State_Description *description) {
    auto pipeline = (Vulkan_Pipeline *)malloc(sizeof(Vulkan_Pipeline));
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;

    std::vector<VkFormat> color_formats;
    color_formats.reserve(description->color_format_count);

    for (u32 i = 0; i < description->color_format_count; i++) {
        color_formats.push_back(vulkan_get_format(description->color_formats[i]));
    }

    VkPipelineRenderingCreateInfo rendering_info = {};
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = static_cast<u32>(color_formats.size());
    rendering_info.pColorAttachmentFormats = color_formats.data();
    rendering_info.depthAttachmentFormat = vulkan_get_depth_format(description->depth_stencil_format);
    rendering_info.stencilAttachmentFormat = vulkan_get_stencil_format(description->depth_stencil_format);
    rendering_info.viewMask = 0x01;

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {};

    for (u32 i = 0; i < description->shader_count; i++) {
        auto shader = (Vulkan_Shader *)description->shaders[i];
        VkPipelineShaderStageCreateInfo Info = {};
        Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        Info.stage = shader->state;
        Info.module = shader->shader;
        Info.pName = "main";

        shader_stages.push_back(Info);
    }

    std::vector<VkVertexInputBindingDescription> vertex_bindings = {};

    for (u32 i = 0; i < description->vertex_input_binding_count; i++) {
        auto binding_description = description->vertex_input_bindings[i];
        VkVertexInputBindingDescription binding = {};
        binding.binding = binding_description.binding;
        binding.stride = binding_description.stride;
        binding.inputRate = vulkan_get_vertex_input_rate(binding_description.input_rate);
        vertex_bindings.push_back(binding);
    }

    std::vector<VkVertexInputAttributeDescription> vertex_attributes = {};

    for (u32 i = 0; i < description->vertex_input_attribute_count; i++) {
        auto attribute_description = description->vertex_input_attributes[i];
        VkVertexInputAttributeDescription attribute = {};
        attribute.location = attribute_description.location;
        attribute.binding = attribute_description.binding;
        attribute.format = vulkan_get_format(attribute_description.format);
        attribute.offset = attribute_description.offset;

        vertex_attributes.push_back(attribute);
    }

    VkPipelineVertexInputStateCreateInfo vertex_input = {};
    vertex_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input.vertexBindingDescriptionCount = static_cast<u32>(vertex_bindings.size());
    vertex_input.pVertexBindingDescriptions = vertex_bindings.data();
    vertex_input.vertexAttributeDescriptionCount = static_cast<u32>(vertex_attributes.size());
    vertex_input.pVertexAttributeDescriptions = vertex_attributes.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.primitiveRestartEnable = false;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineTessellationStateCreateInfo tesselation = {};

    VkPipelineViewportStateCreateInfo viewport = {};
    viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = {};
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f;
    color_blending.blendConstants[1] = 0.0f;
    color_blending.blendConstants[2] = 0.0f;
    color_blending.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pNext = &rendering_info;
    info.stageCount = (u32)shader_stages.size();
    info.pStages = shader_stages.data();
    info.pVertexInputState = &vertex_input;
    info.pInputAssemblyState = &input_assembly;
    info.pTessellationState = &tesselation;
    info.pViewportState = &viewport;
    info.pRasterizationState = &rasterizer;
    info.pMultisampleState = &multisampling;
    info.pDepthStencilState = nullptr;
    info.pColorBlendState = &color_blending;
    info.pDynamicState = &dynamic_state;
    info.layout = ((Vulkan_Pipeline_Layout *)description->layout)->layout;

    if (vkCreateGraphicsPipelines(vulkan_context.device, VK_NULL_HANDLE, 1, &info, nullptr, &pipeline->pipeline) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to create vulkan pipeline");
        return 0;
    }

    return (Rhi_Resource_Handle)pipeline;
}

void vulkan_destroy_pipeline(Rhi_Resource_Handle *resource) {
    auto pipeline = (Vulkan_Pipeline *)*resource;
    vkDestroyPipeline(vulkan_context.device, pipeline->pipeline, nullptr);
    free(pipeline);
    *resource = 0;
}

void vulkan_set_buffer_data(Rhi_Resource_Handle buffer, void *data, u32 size) {
    auto vulkan_buffer = (Vulkan_Buffer *)buffer;
    if (size > vulkan_buffer->size) {
        SK_LOG_WARN("Data is too large for buffer");
        return;
    }

    memcpy(vulkan_buffer->mapped_data, data, size);
}

void vulkan_map_buffer_data(Rhi_Resource_Handle buffer, void **data, u32 size) {
    auto vulkan_buffer = (Vulkan_Buffer *) buffer;
    if (size > vulkan_buffer->size) {
        SK_LOG_WARN("Data is too large for buffer");
        return;
    }
    
    *data = vulkan_buffer->mapped_data;
}

bool vulkan_send_buffer_to_gpu(Rhi_Resource_Handle buffer) {
    auto vulkan_buffer = (Vulkan_Buffer *) buffer;
    if (vulkan_buffer->use_staging_buffer) {
        if (!vulkan_copy_buffer(vulkan_buffer->buffer, vulkan_buffer->staging_buffer, vulkan_buffer->size)) {
            SK_LOG_ERROR("Failed to set buffer data");
            return false;
        }
    }

    return true;
}

Rhi_Resource_Handle vulkan_get_current_swapchain_texture(Rhi_Resource_Handle swapchain) {
    auto vulkan_swapchain = (Vulkan_Swapchain *)swapchain;

    return (Rhi_Resource_Handle)&vulkan_swapchain->images[vulkan_swapchain->image_index];
}

Rhi_Format vulkan_get_texture_format(Rhi_Resource_Handle texture) {
    auto vulkan_texture = (Vulkan_Texture *)texture;
    return vulkan_get_rhi_format(vulkan_texture->format);
}

Rhi_Render_Area vulkan_get_texture_render_area(Rhi_Resource_Handle texture) {
    auto vulkan_texture = (Vulkan_Texture *) texture;
    return vulkan_texture->render_area;
}

bool vulkan_begin_command_list(Rhi_Resource_Handle command_list) {
    auto list = (Vulkan_Command_List *) command_list;

    if (vkResetCommandBuffer(list->buffers[vulkan_context.current_frame], 0) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to reset command buffer");
        return false;
    }

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(list->buffers[vulkan_context.current_frame], &begin_info) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to begin command buffer");
        return false;
    }

    return true;
}

bool vulkan_end_command_list(Rhi_Resource_Handle command_list) {
    auto list = (Vulkan_Command_List *)command_list;

    if (vkEndCommandBuffer(list->buffers[vulkan_context.current_frame]) != VK_SUCCESS) {
        SK_LOG_ERROR("Failed to end command buffer");
        return false;
    }

    return true;
}

void vulkan_cmd_render_imgui_draw_data(Rhi_Resource_Handle command_list, ImDrawData *draw_data) {
    auto list = (Vulkan_Command_List *)command_list;

    ImGui_ImplVulkan_RenderDrawData(draw_data, list->buffers[vulkan_context.current_frame]);
}

void vulkan_cmd_resource_barrier(Rhi_Resource_Handle command_list, Rhi_Resource_Barrier *barrier) {
    auto list = (Vulkan_Command_List *)command_list;

    switch (barrier->type) {
    case Rhi_Barrier_Type::TRANSITION:
        vulkan_transition_barrier(list, &barrier->transition_barrier);
        break;
    default:
        SK_LOG_WARN("Unsupported resource barrier type");
        return;
    }
}

void vulkan_cmd_set_render_target(Rhi_Resource_Handle command_list, Rhi_Resource_Handle target, Rhi_Render_Area *render_area) {
    auto list = (Vulkan_Command_List *)command_list;
    auto vulkan_target = (Vulkan_Texture *)target;

    VkRenderingAttachmentInfo attachment_info = {};
    attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachment_info.clearValue.color.float32[0] = 1.0f;
    attachment_info.clearValue.color.float32[1] = 0.0f;
    attachment_info.clearValue.color.float32[2] = 1.0f;
    attachment_info.clearValue.color.float32[3] = 1.0f;
    attachment_info.imageView = vulkan_target->image_view;
    attachment_info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo rendering_info = {};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments = &attachment_info;
    rendering_info.viewMask = 0x01;
    rendering_info.layerCount = render_area->layer_count;
    rendering_info.renderArea.extent = { render_area->x, render_area->y };
    rendering_info.renderArea.extent = { render_area->width, render_area->height };
    vkCmdBeginRendering(list->buffers[vulkan_context.current_frame], &rendering_info);
}

void vulkan_cmd_unset_render_target(Rhi_Resource_Handle command_list) {
    auto list = (Vulkan_Command_List *)command_list;


    vkCmdEndRendering(list->buffers[vulkan_context.current_frame]);
}

void vulkan_cmd_bind_vertex_buffer(Rhi_Resource_Handle command_list, Rhi_Resource_Handle buffer, u32 first_vertex) {
    auto list = (Vulkan_Command_List *)command_list;
    auto vulkan_buffer = (Vulkan_Buffer *) buffer;

    VkDeviceSize offset = first_vertex;

    vkCmdBindVertexBuffers(list->buffers[vulkan_context.current_frame], 0, 1, &vulkan_buffer->buffer, &offset);
}

void vulkan_cmd_bind_index_buffer(Rhi_Resource_Handle command_list, Rhi_Resource_Handle buffer) {
    auto list = (Vulkan_Command_List *)command_list;
    auto vulkan_buffer = (Vulkan_Buffer *)buffer;

    vkCmdBindIndexBuffer(list->buffers[vulkan_context.current_frame], vulkan_buffer->buffer, 0, VK_INDEX_TYPE_UINT16);
}

void vulkan_cmd_draw_indexed(Rhi_Resource_Handle command_list, u32 index_count, u32 instance_count, u32 first_index, s32 vertex_offset, u32 first_instance) {
    auto list = (Vulkan_Command_List *)command_list;

    vkCmdDrawIndexed(list->buffers[vulkan_context.current_frame], index_count, instance_count, first_index, vertex_offset, first_instance);
}

void vulkan_cmd_draw_instanced(Rhi_Resource_Handle command_list, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) {
    auto list = (Vulkan_Command_List *)command_list;

    vkCmdDraw(list->buffers[vulkan_context.current_frame], vertex_count, instance_count, first_vertex, first_instance);
}

void vulkan_cmd_set_viewport(Rhi_Resource_Handle command_list, f32 x, f32 y, f32 width, f32 height, f32 min_depth, f32 max_depth) {
    auto list = (Vulkan_Command_List *)command_list;

    VkViewport viewport = {};
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = min_depth;
    viewport.maxDepth = max_depth;

    vkCmdSetViewport(list->buffers[vulkan_context.current_frame], 0, 1, &viewport);
}

void vulkan_cmd_set_scissor(Rhi_Resource_Handle command_list, s32 x, s32 y, u32 width, u32 height) {
    auto list = (Vulkan_Command_List *)command_list;

    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    scissor.offset.x = x;
    scissor.offset.x = y;

    vkCmdSetScissor(list->buffers[vulkan_context.current_frame], 0, 1, &scissor);
}

void vulkan_cmd_bind_pipeline(Rhi_Resource_Handle command_list, Rhi_Resource_Handle pipeline) {
    auto list = (Vulkan_Command_List *)command_list;
    auto vulkan_pipeline = (Vulkan_Pipeline *) pipeline;

    vkCmdBindPipeline(list->buffers[vulkan_context.current_frame], vulkan_pipeline->bind_point, vulkan_pipeline->pipeline);
}

void vulkan_notify_swapchain_of_resize(Rhi_Resource_Handle swapchain) {
    auto vulkan_swapchain = (Vulkan_Swapchain *)swapchain;

    vulkan_swapchain->framebuffer_resized = true;
}