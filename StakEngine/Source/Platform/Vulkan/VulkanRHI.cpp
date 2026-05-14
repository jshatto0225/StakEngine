#include "VulkanRHI.h"

#include "Log.h"

#include <vulkan/vulkan.h>

#include "VulkanPlatform.h"

enum VulkanValidationSeverity {
    VULKAN_VALIDATION_SEVERITY_TRACE, // NOTE: For technically incorrect but safe operations (vk_free on a nullptr). Behavior is defined but not recommended.
    VULKAN_VALIDATION_SEVERITY_INFO, // NOTE: Information (usually object creation or other sucess, and metrics/statistics)
    VULKAN_VALIDATION_SEVERITY_WARN, // NOTE: For incorrect operations that may not be safe (recording to a null command buffer). Think undefined behavior.
    VULKAN_VALIDATION_SEVERITY_ERROR, // NOTE: Internal api errors
    VULKAN_VALIDATION_SEVERITY_CRITICAL // NOTE: Invalid or unsafe state (gpu or cpu pointer that is null or not allocated from vk_alloc). Causes an immediate crash.
};

#ifdef SK_DEBUG
static const bool enable_validation = true;
static const VulkanValidationSeverity validation_level = VULKAN_VALIDATION_SEVERITY_TRACE;
const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};
const u32 validation_layer_count = 1;
#endif



#define VULKAN_VALIDATE(cond, msg, severity) { if (!(cond)) vk_validation_message(msg, severity); if (severity == VULKAN_VALIDATION_SEVERITY_CRITICAL) assert(false); }

struct AllocBlock {
    VkBuffer buffer;
    VkDeviceMemory memory;

    u64 size;
    
    void *cpu;
    void *gpu;
};

struct Vulkan {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
};

struct VulkanSwapchain {
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;

    u32 image_count;
    std::vector<VkImage> images;
    std::vector<VkImageView> image_views;
    VkFormat format;
    VkExtent2D extent;
};

struct VulkanDevice {
    VkDevice device;
    VkPhysicalDevice gpu;
    VkCommandPool command_pool;

    std::vector<AllocBlock> cpu_allocations;
    std::vector<AllocBlock> gpu_allocations;
};

struct VulkanSemaphore {
    VkSemaphore semaphore;
};

struct VulkanQueue {
    VulkanDevice *device;
    VkCommandPool command_pool;
    VkQueue queue;
};

struct VulkanTexture {
    VkImage image;
    VkImageView image_view;
    VkFormat format;
    VkExtent3D extent;
    u32 layers;
    u32 mip_count;
    u32 sample_count;
    VkImageUsageFlags usage;
    VkImageType type;
    VkImageLayout layout;
    VkImageAspectFlags aspect_mask;
};

struct VulkanCommandBuffer {
    VkCommandBuffer command_buffer;
    VulkanQueue *queue;
    std::vector<VkSemaphoreSubmitInfo> waits;
    std::vector<VkSemaphoreSubmitInfo> signals;
};

struct VulkanPipeline {
    VkPipeline pipeline;
    VkPipelineBindPoint bind_point;
};

struct VulkanRasterPushConstants {
    void *vert_data;
    void *frag_data;
};

struct VulkanMeshPushConstants {
    void *mesh_data;
    void *frag_data;
};

struct VulkanComputePushConstants {
    void *data;
};

struct VulkanBufferOffset {
    VkBuffer buffer;
    VkDeviceSize offset;
};

struct VulkanBlendState {
    VkBlendFactor src_color_factor;
    VkBlendFactor dst_color_factor;
    VkBlendOp color_op;
    VkBlendFactor src_alpha_factor;
    VkBlendFactor dst_alpha_factor;
    VkBlendOp alpha_op;
    VkColorComponentFlags color_write_mask;
};

struct VulkanStencil {
    u32 write_mask;
    VkStencilOp fail_op;
    VkStencilOp pass_op;
    VkStencilOp depth_fail_op;
    VkCompareOp compare_op;
    u32 reference;
    u32 compare_mask;
};

struct VulkanDepthStencilState {
    bool depth_write_enabled;
    VkCompareOp depth_compare_op;
    float depth_bias_constant;
    float depth_bias_clamp;
    float depth_bias_slope;
    VulkanStencil front;
    VulkanStencil back;
};

Vulkan vulkan;

VkResult create_debug_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *info, const VkAllocationCallbacks *allocator, VkDebugUtilsMessengerEXT *messenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, info, allocator, messenger);
    } else {
        SK_LOG_ERROR("Extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *allocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, messenger, allocator);
    }
}

static void vk_validation_message(const char *str, VulkanValidationSeverity severity) {
    if (severity >= validation_level) {
        switch (severity) {
            case VULKAN_VALIDATION_SEVERITY_TRACE:
                SK_LOG_TRACE("[RHI_VALIDATION] {}", str);
                break;
            case VULKAN_VALIDATION_SEVERITY_INFO:
                SK_LOG_INFO("[RHI_VALIDATION] {}", str);
                break;
            case VULKAN_VALIDATION_SEVERITY_WARN:
                SK_LOG_WARN("[RHI_VALIDATION] {}", str);
                break;
            case VULKAN_VALIDATION_SEVERITY_ERROR:
                SK_LOG_ERROR("[RHI_VALIDATION] {}", str);
                break;
            case VULKAN_VALIDATION_SEVERITY_CRITICAL:
                SK_LOG_CRITICAL("[RHI_VALIDATION] {}", str);
                break;
        }
    }
}

VkStencilOp vk_get_stencil_op(RHIStencilOp op) {
    switch (op) {
        case RHI_STENCIL_OP_KEEP:
            return VK_STENCIL_OP_KEEP;
        case RHI_STENCIL_OP_ZERO:
            return VK_STENCIL_OP_ZERO;
        case RHI_STENCIL_OP_REPLACE:
            return VK_STENCIL_OP_REPLACE;
        case RHI_STENCIL_OP_INCREMENT_AND_CLAMP:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case RHI_STENCIL_OP_DECREMENT_AND_CLAMP:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case RHI_STENCIL_OP_INVERT:
            return VK_STENCIL_OP_INVERT;
        case RHI_STENCIL_OP_INCREMENT_AND_WRAP:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case RHI_STENCIL_OP_DECREMENT_AND_WRAP:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            VULKAN_VALIDATE(false, "vk_get_compare_op called with invalid op", VULKAN_VALIDATION_SEVERITY_WARN);
            return (VkStencilOp) 0;
    }
}

VkCompareOp vk_get_compare_op(RHIOp op) {
    switch (op) {
        case RHI_OP_NEVER:
            return VK_COMPARE_OP_NEVER;
        case RHI_OP_LESS:
            return VK_COMPARE_OP_LESS;
        case RHI_OP_EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case RHI_OP_LESS_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case RHI_OP_GREATER:
            return VK_COMPARE_OP_GREATER;
        case RHI_OP_NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case RHI_OP_GREATER_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case RHI_OP_ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        default:
            VULKAN_VALIDATE(false, "vk_get_compare_op called with invalid op", VULKAN_VALIDATION_SEVERITY_WARN);
            return (VkCompareOp) 0;
    }
}

VkPipelineStageFlags2 vk_get_pipeline_stage(RHIPipelineStage stage) {
    switch (stage) {
        case RHI_PIPELINE_STAGE_COMPUTE:
            return VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        case RHI_PIPELINE_STAGE_PIXEL_SHADER:
            return VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        case RHI_PIPELINE_STAGE_RASTER_COLOR_OUT:
            return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        case RHI_PIPELINE_STAGE_TRANSFER:
            return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        case RHI_PIPELINE_STAGE_VERTEX_SHADER:
            return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
        default:
            VULKAN_VALIDATE(false, "vk_get_pipeline_stage called with invalid pipeline stage", VULKAN_VALIDATION_SEVERITY_WARN);
            return 0;
    }
}

VkPrimitiveTopology vk_get_topology(RHITopology t) {
    switch (t) {
        case RHI_TOPOLOGY_TRIANGLE_FAN:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case RHI_TOPOLOGY_TRIANGLE_LIST:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case RHI_TOPOLOGY_TRIANGLE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        default:
            VULKAN_VALIDATE(false, "vk_get_topology called with invalid topology", VULKAN_VALIDATION_SEVERITY_WARN);
            return (VkPrimitiveTopology) 0;
    }
}

VkBlendFactor vk_get_blend_factor(RHIBlendFactor factor) {
    switch (factor) {
        case RHI_BLEND_FACTOR_DST_COLOR:
            return VK_BLEND_FACTOR_DST_COLOR;
        case RHI_BLEND_FACTOR_ONE:
            return VK_BLEND_FACTOR_ONE;
        case RHI_BLEND_FACTOR_SRC_ALPHA:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case RHI_BLEND_FACTOR_SRC_COLOR:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case RHI_BLEND_FACTOR_ZERO:
            return VK_BLEND_FACTOR_ZERO;
        default:
            VULKAN_VALIDATE(false, "vk_get_blend_factor called with invalid factor", VULKAN_VALIDATION_SEVERITY_WARN);
            return (VkBlendFactor) 0;
    }
}

VkBlendOp vk_get_blend_op(RHIBlendOp op) {
    switch (op) {
        case RHI_BLEND_OP_ADD:
            return VK_BLEND_OP_ADD;
        case RHI_BLEND_OP_MAX:
            return VK_BLEND_OP_MAX;
        case RHI_BLEND_OP_MIN:
            return VK_BLEND_OP_MIN;
        case RHI_BLEND_OP_REV_SUBTRACT:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case RHI_BLEND_OP_SUBTRACT:
            return VK_BLEND_OP_SUBTRACT;
        default:
            VULKAN_VALIDATE(false, "vk_get_blend_op called with invalid op", VULKAN_VALIDATION_SEVERITY_WARN);
            return (VkBlendOp) 0;
    }
}

VkCullModeFlags vk_get_cull_mode(RHICullMode mode) {
    switch (mode) {
        case RHI_CULL_MODE_ALL:
            return VK_CULL_MODE_FRONT_AND_BACK;
        case RHI_CULL_MODE_NONE:
            return VK_CULL_MODE_NONE;
        case RHI_CULL_MODE_CCW:
            return VK_CULL_MODE_FRONT_BIT;
        case RHI_CULL_MODE_CW:
            return VK_CULL_MODE_BACK_BIT;
        default:
            VULKAN_VALIDATE(false, "vk_get_cull_mode called with invalid mode", VULKAN_VALIDATION_SEVERITY_WARN);
            return (VkCullModeFlags) 0;
    }
}

AllocBlock *find_allocation_gpu(VulkanDevice *device, void *gpu) {
    VULKAN_VALIDATE(device != nullptr, "find_allocation_gpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (device == nullptr) {
        return nullptr;
    }

    u64 low = 0;
    u64 high = device->gpu_allocations.size() - 1;

    while (low <= high) {
        u64 mid = low + (high - low) / 2;

        auto block = &device->gpu_allocations[mid];
        if ((u64) gpu >= (u64) block->gpu && (u64) gpu < (u64) block->gpu + block->size) {
            return block;
        }

        if ((u64) gpu < (u64) block->gpu) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    return nullptr;
}

AllocBlock *find_allocation_cpu(VulkanDevice *device, void *cpu) {
    VULKAN_VALIDATE(device != nullptr, "find_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (device == nullptr) {
        return nullptr;
    }

    u64 low = 0;
    u64 high = device->cpu_allocations.size() - 1;

    while (low <= high) {
        u64 mid = low + (high - low) / 2;

        auto block = &device->cpu_allocations[mid];
        if ((u64) cpu >= (u64) block->cpu && (u64) cpu < (u64) block->cpu + block->size) {
            return block;
        }

        if ((u64) cpu < (u64) block->cpu) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    return nullptr;
}

VulkanBufferOffset get_buffer_offset_gpu(VulkanDevice *device, void *gpu) {
    VULKAN_VALIDATE(device != nullptr, "get_buffer_offset_gpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    AllocBlock *block = find_allocation_gpu(device, gpu);

    VULKAN_VALIDATE(block != nullptr, "get_buffer_offset_gpu could not find associated allocation block", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    u64 offset = (u64) gpu - (u64) block->gpu;

    return { block->buffer, offset };
}

bool find_memory_type(VkPhysicalDevice gpu, u32 *memory_type_index, u32 filter, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(gpu, &mem_props);

    for (auto i = 0; i < mem_props.memoryTypeCount; i++) {
        if (((filter & (1 << i)) != 0) && ((mem_props.memoryTypes[i].propertyFlags & flags) == flags)) {
            *memory_type_index = i;
            return true;
        }
    }

    VULKAN_VALIDATE(false, "find_memory_type Failed to find memory type", VULKAN_VALIDATION_SEVERITY_ERROR);

    return false;
}

u64 find_insert_pos_cpu(VulkanDevice *device, void *cpu) {
    u64 low = 0;
    u64 high = device->cpu_allocations.size();

    while (low < high) {
        u64 mid = (low + high) / 2;

        if (device->cpu_allocations[mid].cpu < cpu) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low;
}

u64 find_insert_pos_gpu(VulkanDevice *device, void *gpu) {
    u64 low = 0;
    u64 high = device->gpu_allocations.size();

    while (low < high) {
        u64 mid = (low + high) / 2;

        if (device->gpu_allocations[mid].gpu < gpu) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low;
}

void insert_allocation_cpu(VulkanDevice *device, AllocBlock in) {
    VULKAN_VALIDATE(device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (device == nullptr) {
        return;
    }

    auto pos = find_insert_pos_cpu(device, in.cpu);
    device->cpu_allocations.insert(device->cpu_allocations.begin() + pos, in);
}

void insert_allocation_gpu(VulkanDevice *device, AllocBlock in) {
    VULKAN_VALIDATE(device != nullptr, "insert_allocation_gpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (device == nullptr) {
        return;
    }

    auto pos = find_insert_pos_gpu(device, in.gpu);
    device->gpu_allocations.insert(device->gpu_allocations.begin() + pos, in);
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, 
    VkDebugUtilsMessageTypeFlagsEXT type, 
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data, 
    void *user_data) {
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

bool vulkan_init(RHI *rhi) {
    bool extensions_supported = false;
    u32 layer_count = 0;

    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    auto layers = (VkLayerProperties *) malloc(sizeof(VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, layers);

    for (u32 i = 0; i < validation_layer_count; i++) {
        bool layer_found = false;

        for (u32 k = 0; k < layer_count; k++) {
            if (strcmp(validation_layers[i], layers[k].layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            extensions_supported = false;
            break;
        }
    }

    VULKAN_VALIDATE(extensions_supported, "vulkan_init Vulkan extensions are not supported", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "app",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "engine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_4,
    };

    VkInstanceCreateInfo instance_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
    };

    VkDebugUtilsMessengerCreateInfoEXT debug_info = {};

    if (enable_validation) {
        instance_info.enabledLayerCount = validation_layer_count;
        instance_info.ppEnabledLayerNames = validation_layers;

        debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_info.pfnUserCallback = debug_messenger;

        instance_info.pNext = &debug_info;
    }

    std::vector<const char *> extensions = platform_get_required_extensions();
    if (enable_validation) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instance_info.enabledExtensionCount = (u32) extensions.size();
    instance_info.ppEnabledExtensionNames = extensions.data();

    if (vkCreateInstance(&instance_info, nullptr, &vulkan.instance) != VK_SUCCESS) {
        VULKAN_VALIDATE(extensions_supported, "vulkan_init vkCreateInstance failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return false;
    }
    
    if (create_debug_messenger(vulkan.instance, &debug_info, nullptr, &vulkan.debug_messenger) != VK_SUCCESS) {
        VULKAN_VALIDATE(extensions_supported, "vulkan_init create_debug_messenger failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return false;
    }

    *rhi = {
        // Memory
        vk_alloc,
        vk_free,
        vk_host_to_device_pointer,

        // Device
        vk_create_device,
        vk_destroy_device,
        vk_device_wait_idle,

        //Textures
        vk_texture_size_align,
        vk_create_texture,
        vk_destroy_texture,
        vk_texture_view_descriptor,
        vk_rw_texture_view_descriptor,

        // Pipelines
        vk_create_compute_pipeline,
        vk_create_graphics_pipeline,
        vk_create_graphics_meshlet_pipeline,
        vk_destroy_pipeline,

        // State objects
        vk_create_depth_stencil_state,
        vk_create_blend_state,
        vk_free_depth_stencil_state,
        vk_free_blend_state,

        // Queue
        vk_create_queue,
        vk_destroy_queue,
        vk_start_command_recording,
        vk_submit,

        // Semaphores
        vk_create_semaphore,
        vk_wait_semaphore,
        vk_destroy_semaphore,

        // Commands
        vk_mem_copy,
        vk_copy_to_texture,
        vk_copy_from_texture,

        vk_set_active_texture_heap_ptr,
        vk_set_active_resource_heap_ptr,

        vk_barrier,
        vk_signal_after,
        vk_wait_before,

        vk_set_pipeline,
        vk_set_depth_stencil_state,
        vk_set_blend_state,

        vk_dispatch,
        vk_dispatch_indirect,

        vk_begin_render_pass,
        vk_end_render_pass,

        vk_draw_indexed_instanced,
        vk_draw_indexed_instanced_indirect,
        vk_draw_indexed_instanced_indirect_multi,

        vk_draw_meshlets,
        vk_draw_meshlets_indirect,
    };

    return true;
}

void vulkan_shutdown() {
    destroy_debug_messenger(vulkan.instance, vulkan.debug_messenger, nullptr);
    vkDestroyInstance(vulkan.instance, nullptr);
}

// Memory
void *vk_alloc(RHIDevice device, u64 bytes, RHIMemoryType memory = RHI_MEMORY_TYPE_DEFAULT) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "vk_alloc called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return nullptr;
    }

    VULKAN_VALIDATE(bytes > 0, "vk_alloc given 0 size", VULKAN_VALIDATION_SEVERITY_TRACE);

    if (bytes == 0) {
        return nullptr;
    }

    AllocBlock alloc_block = {};
    alloc_block.size = bytes;
    alloc_block.cpu = nullptr;

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bytes,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    if (vkCreateBuffer(vulkan_device->device, &buffer_info, nullptr, &alloc_block.buffer) != VK_SUCCESS) {
        return nullptr;
    }

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(vulkan_device->device, alloc_block.buffer, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = mem_reqs.size
    };

    VkMemoryPropertyFlags properties = {};
    switch (memory) {
        case RHI_MEMORY_TYPE_GPU:
            properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case RHI_MEMORY_TYPE_DEFAULT:
            properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case RHI_MEMORY_TYPE_READBACK:
            properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;
        default:
            vkDestroyBuffer(vulkan_device->device, alloc_block.buffer, nullptr);
            VULKAN_VALIDATE(false, "vk_alloc invalid RHIMemoryType", VULKAN_VALIDATION_SEVERITY_ERROR);
            return nullptr;
    }

    if (!find_memory_type(vulkan_device->gpu, &alloc_info.memoryTypeIndex, mem_reqs.memoryTypeBits, properties) != VK_SUCCESS) {
        vkDestroyBuffer(vulkan_device->device, alloc_block.buffer, nullptr);

        VULKAN_VALIDATE(false, "vk_alloc find_memory_type failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return nullptr;
    }

    if (vkAllocateMemory(vulkan_device->device, &alloc_info, nullptr, &alloc_block.memory) != VK_SUCCESS) {
        vkDestroyBuffer(vulkan_device->device, alloc_block.buffer, nullptr);

        VULKAN_VALIDATE(false, "vk_alloc vkAllocateMemory failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return nullptr;
    }

    VkBufferDeviceAddressInfo addr_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = alloc_block.buffer,
    };
    alloc_block.gpu = (void *)vkGetBufferDeviceAddress(vulkan_device->device, &addr_info);

    if (memory == RHI_MEMORY_TYPE_DEFAULT || memory == RHI_MEMORY_TYPE_READBACK) {
        if (vkMapMemory(vulkan_device->device, alloc_block.memory, 0, alloc_block.size, 0, &alloc_block.cpu) != VK_SUCCESS) {
            vkFreeMemory(vulkan_device->device, alloc_block.memory, nullptr);
            vkDestroyBuffer(vulkan_device->device, alloc_block.buffer, nullptr);

            VULKAN_VALIDATE(false, "vk_alloc vkMapMemory failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        }

        insert_allocation_cpu(vulkan_device, alloc_block);
        insert_allocation_gpu(vulkan_device, alloc_block);
        return alloc_block.cpu;
    }

    insert_allocation_gpu(vulkan_device, alloc_block);
    return alloc_block.gpu;
}

void vk_free(RHIDevice device, void *ptr) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "vk_free called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return;
    }

    VULKAN_VALIDATE(ptr != nullptr, "vk_free called with nullptr", VULKAN_VALIDATION_SEVERITY_TRACE);

    if (ptr == nullptr) {
        return;
    }

    AllocBlock *block = find_allocation_cpu(vulkan_device, ptr);
    if (!block) {
        block = find_allocation_gpu(vulkan_device, ptr);
    }

    VULKAN_VALIDATE(block != nullptr, "vk_free called with invalid pointer", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (block->cpu) {
        vkUnmapMemory(vulkan_device->device, block->memory);
    }
    vkFreeMemory(vulkan_device->device, block->memory, nullptr);
    vkDestroyBuffer(vulkan_device->device, block->buffer, nullptr);
}

void *vk_host_to_device_pointer(RHIDevice device, void *ptr) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "vk_host_to_device_pointer called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return nullptr;
    }

    AllocBlock *block = find_allocation_cpu(vulkan_device, ptr);

    VULKAN_VALIDATE(block != nullptr, "vk_host_to_device_pointer called with invalid pointer", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    u64 diff = (u64) ptr - (u64) block->cpu;

    return (void *) ((u64) block->gpu + (u64) diff);
}

// Textures
RHITextureSizeAlign vk_texture_size_align(RHIDevice device, RHITextureDesc *desc);

RHITexture vk_create_texture(RHIDevice device, RHITextureDesc *desc, void *ptr_gpu);

void vk_destroy_texture(RHIDevice device, RHITexture texture);

RHITextureDescriptor vk_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc);

RHITextureDescriptor vk_rw_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc);

// Pipelines
RHIPipeline vk_create_compute_pipeline(RHIDevice device, u8 *compute_ir, u32 ir_size) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return 0;
    }

    VULKAN_VALIDATE(compute_ir != nullptr, "vk_create_compute_pipeline called with null compute_ir", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(ir_size > 0, "vk_create_compute_pipeline called with 0 ir_size", VULKAN_VALIDATION_SEVERITY_WARN);

    if (compute_ir == nullptr || ir_size == 0) {
        return 0;
    }

    VkShaderModuleCreateInfo shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = ir_size,
        .pCode = (u32 *) compute_ir,
    };

    VkShaderModule shader_module = nullptr;

    if (vkCreateShaderModule(vulkan_device->device, &shader_info, nullptr, &shader_module) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_create_compute_pipeline vkCreateShaderModule failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return 0;
    }

    VkComputePipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = shader_module,
            .pName = "main",
        }
    };

    VkPipeline vulkan_pipeline = nullptr;
    if (vkCreateComputePipelines(vulkan_device->device, nullptr, 1, &info, nullptr, &vulkan_pipeline) != VK_SUCCESS) {
        vkDestroyShaderModule(vulkan_device->device, shader_module, nullptr);

        VULKAN_VALIDATE(false, "vk_create_compute_pipeline vkCreateComputePipelines failed", VULKAN_VALIDATION_SEVERITY_ERROR);

        return 0;
    }

    vkDestroyShaderModule(vulkan_device->device, shader_module, nullptr);

    auto pipeline = (VulkanPipeline *) malloc(sizeof(VulkanPipeline));
    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
    return (u64) pipeline;
}

RHIPipeline vk_create_graphics_pipeline(RHIDevice device, u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return 0;
    }

    VULKAN_VALIDATE(vertex_ir != nullptr, "vk_create_graphics_pipeline called with null vertex_ir", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(vertex_ir_size > 0, "vk_create_graphics_pipeline called with 0 vertex_ir_size", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(pixel_ir != nullptr, "vk_create_graphics_pipeline called with null pixel_ir", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(pixel_ir_size > 0, "vk_create_graphics_pipeline called with 0 pixel_ir_size", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vertex_ir == nullptr || pixel_ir == nullptr || vertex_ir_size == 0 || pixel_ir_size == 0) {
        return 0;
    }

    VkShaderModuleCreateInfo vertex_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = vertex_ir_size,
        .pCode = (u32 *) vertex_ir,
    };

    VkShaderModuleCreateInfo fragment_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = pixel_ir_size,
        .pCode = (u32 *) pixel_ir,
    };

    VkShaderModule vertex_shader_module = nullptr;
    VkShaderModule fragment_shader_module = nullptr;

    if (vkCreateShaderModule(vulkan_device->device, &vertex_shader_info, nullptr, &vertex_shader_module) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_create_graphics_pipeline vkCreateShaderModule failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return 0;
    }

    if (vkCreateShaderModule(vulkan_device->device, &vertex_shader_info, nullptr, &fragment_shader_module) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_create_graphics_pipeline vkCreateShaderModule failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return 0;
    }

    const u32 shader_stage_count = 2;
    VkPipelineShaderStageCreateInfo shader_stages[shader_stage_count] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertex_shader_module,
            .pName = "main",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragment_shader_module,
            .pName = "main",
        }
    };

    struct RHIRasterDesc {
        bool support_dual_source_blending;
        RHIFormat depth_format;
        RHIFormat stencil_format;
        RHIColorTarget *color_targets;
        u32 color_target_count;
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = vk_get_topology(desc->topology),
        .primitiveRestartEnable = false,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = vk_get_cull_mode(desc->cull),
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = (VkSampleCountFlagBits) desc->sample_count,
        .alphaToCoverageEnable = desc->alpha_to_coverage,
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {};
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};

    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,

        VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
        VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
        VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE,
        VK_DYNAMIC_STATE_DEPTH_BIAS,
        VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
        VK_DYNAMIC_STATE_STENCIL_OP,
        VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
    };

    VkGraphicsPipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = shader_stage_count,
        .pStages = shader_stages,
        .pInputAssemblyState = &input_assembly,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
    };

    if (desc->blend_state) {
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;

        color_blend_attachment.blendEnable = true;
        color_blend_attachment.colorBlendOp = vk_get_blend_op(desc->blend_state->color_op);
        color_blend_attachment.srcColorBlendFactor = vk_get_blend_factor(desc->blend_state->src_color_factor);
        color_blend_attachment.dstColorBlendFactor = vk_get_blend_factor(desc->blend_state->dst_color_factor);
        color_blend_attachment.colorBlendOp = vk_get_blend_op(desc->blend_state->alpha_op);
        color_blend_attachment.srcColorBlendFactor = vk_get_blend_factor(desc->blend_state->src_alpha_factor);
        color_blend_attachment.dstColorBlendFactor = vk_get_blend_factor(desc->blend_state->dst_alpha_factor);
        color_blend_attachment.colorWriteMask = desc->blend_state->color_write_mask;

        info.pColorBlendState = &color_blending;
    } else {
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT);
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT);
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT);
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT);
    }

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (u32) dynamic_states.size(),
        .pDynamicStates = dynamic_states.data(),
    };
    info.pDynamicState = &dynamic_state;

    VkPipeline vulkan_pipeline = nullptr;
    if (vkCreateGraphicsPipelines(vulkan_device->device, nullptr, 1, &info, nullptr, &vulkan_pipeline) != VK_SUCCESS) {
        vkDestroyShaderModule(vulkan_device->device, vertex_shader_module, nullptr);
        vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

        VULKAN_VALIDATE(false, "vk_create_graphics_pipeline vkCreateGraphicsPipelines failed", VULKAN_VALIDATION_SEVERITY_ERROR);

        return 0;
    }

    vkDestroyShaderModule(vulkan_device->device, vertex_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

    auto pipeline = (VulkanPipeline *) malloc(sizeof(VulkanPipeline));
    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return (u64) pipeline;
}

RHIPipeline vk_create_graphics_meshlet_pipeline(RHIDevice device, u8 *meshlet_ir, u32 meshlet_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return 0;
    }

    VULKAN_VALIDATE(meshlet_ir != nullptr, "vk_create_graphics_meshlet_pipeline called with null meshlet_ir", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(meshlet_ir_size > 0, "vk_create_graphics_meshlet_pipeline called with 0 meshlet_ir_size", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(pixel_ir != nullptr, "vk_create_graphics_meshlet_pipeline called with null pixel_ir", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(pixel_ir_size > 0, "vk_create_graphics_meshlet_pipeline called with 0 pixel_ir_size", VULKAN_VALIDATION_SEVERITY_WARN);

    if (meshlet_ir == nullptr || pixel_ir == nullptr || meshlet_ir_size == 0 || pixel_ir_size == 0) {
        return 0;
    }

    VkShaderModuleCreateInfo mesh_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = meshlet_ir_size,
        .pCode = (u32 *) meshlet_ir,
    };

    VkShaderModuleCreateInfo fragment_shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = pixel_ir_size,
        .pCode = (u32 *) pixel_ir,
    };

    VkShaderModule mesh_shader_module = nullptr;
    VkShaderModule fragment_shader_module = nullptr;

    if (vkCreateShaderModule(vulkan_device->device, &mesh_shader_info, nullptr, &mesh_shader_module) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_create_graphics_meshlet_pipeline vkCreateShaderModule failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return 0;
    }

    if (vkCreateShaderModule(vulkan_device->device, &fragment_shader_info, nullptr, &fragment_shader_module) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_create_graphics_meshlet_pipeline vkCreateShaderModule failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return {};
    }

    const u32 shader_stage_count = 2;
    VkPipelineShaderStageCreateInfo shader_stages[shader_stage_count] = {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_MESH_BIT_EXT,
            .module = mesh_shader_module,
            .pName = "main",
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragment_shader_module,
            .pName = "main",
        }
    };

    struct RHIRasterDesc {
        bool support_dual_source_blending;
        RHIFormat depth_format;
        RHIFormat stencil_format;
        RHIColorTarget *color_targets;
        u32 color_target_count;
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = vk_get_topology(desc->topology),
        .primitiveRestartEnable = false,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = vk_get_cull_mode(desc->cull),
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = (VkSampleCountFlagBits) desc->sample_count,
        .alphaToCoverageEnable = desc->alpha_to_coverage,
    };

    VkPipelineColorBlendStateCreateInfo color_blending = {};
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};

    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,

        VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
        VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
        VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE,
        VK_DYNAMIC_STATE_DEPTH_BIAS,
        VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
        VK_DYNAMIC_STATE_STENCIL_OP,
        VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
    };

    VkGraphicsPipelineCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = shader_stage_count,
        .pStages = shader_stages,
        .pInputAssemblyState = &input_assembly,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
    };

    if (desc->blend_state) {
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments = &color_blend_attachment;

        color_blend_attachment.blendEnable = true;
        color_blend_attachment.colorBlendOp = vk_get_blend_op(desc->blend_state->color_op);
        color_blend_attachment.srcColorBlendFactor = vk_get_blend_factor(desc->blend_state->src_color_factor);
        color_blend_attachment.dstColorBlendFactor = vk_get_blend_factor(desc->blend_state->dst_color_factor);
        color_blend_attachment.colorBlendOp = vk_get_blend_op(desc->blend_state->alpha_op);
        color_blend_attachment.srcColorBlendFactor = vk_get_blend_factor(desc->blend_state->src_alpha_factor);
        color_blend_attachment.dstColorBlendFactor = vk_get_blend_factor(desc->blend_state->dst_alpha_factor);
        color_blend_attachment.colorWriteMask = desc->blend_state->color_write_mask;

        info.pColorBlendState = &color_blending;
    } else {
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT);
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT);
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT);
        dynamic_states.push_back(VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT);
    }

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = (u32) dynamic_states.size(),
        .pDynamicStates = dynamic_states.data(),
    };
    info.pDynamicState = &dynamic_state;

    VkPipeline vulkan_pipeline;
    if (vkCreateGraphicsPipelines(vulkan_device->device, nullptr, 1, &info, nullptr, &vulkan_pipeline) != VK_SUCCESS) {
        vkDestroyShaderModule(vulkan_device->device, mesh_shader_module, nullptr);
        vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

        VULKAN_VALIDATE(false, "vk_create_graphics_meshlet_pipeline vkCreateGraphicsPipelines failed", VULKAN_VALIDATION_SEVERITY_ERROR);

        return 0;
    }

    vkDestroyShaderModule(vulkan_device->device, mesh_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

    auto pipeline = (VulkanPipeline *) malloc(sizeof(VulkanPipeline));
    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return (u64) pipeline;
}

void vk_destroy_pipeline(RHIDevice device, RHIPipeline pipeline) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return;
    }

    auto vulkan_pipeline = (VulkanPipeline *) pipeline;

    VULKAN_VALIDATE(vulkan_pipeline != nullptr, "vk_free_pipeline called with empty pipeline", VULKAN_VALIDATION_SEVERITY_TRACE);

    if (vulkan_pipeline == nullptr) {
        return;
    }

    vkDestroyPipeline(vulkan_device->device, vulkan_pipeline->pipeline, nullptr);
    free(vulkan_pipeline);
}

// State objects
RHIDepthStencilState vk_create_depth_stencil_state(RHIDevice device, RHIDepthStencilDesc *desc) {
    auto state = (VulkanDepthStencilState *) malloc(sizeof(VulkanDepthStencilState));

    state->depth_write_enabled = desc->depth_mode == RHI_DEPTH_WRITE;
    state->depth_compare_op = vk_get_compare_op(desc->depth_test);
    state->depth_bias_constant = desc->depth_bias;
    state->depth_bias_clamp = desc->depth_bias_clamp;
    state->depth_bias_slope = desc->depth_bias_slope_factor;
    
    state->front.write_mask = desc->stencil_write_mask;
    state->front.fail_op = vk_get_stencil_op(desc->stencil_front.fail_op);
    state->front.pass_op = vk_get_stencil_op(desc->stencil_front.pass_op);
    state->front.depth_fail_op = vk_get_stencil_op(desc->stencil_front.depth_fail_op);
    state->front.reference = desc->stencil_front.reference;
    state->front.compare_mask = desc->stencil_read_mask;

    state->back.write_mask = desc->stencil_write_mask;
    state->back.fail_op = vk_get_stencil_op(desc->stencil_back.fail_op);
    state->back.pass_op = vk_get_stencil_op(desc->stencil_back.pass_op);
    state->back.depth_fail_op = vk_get_stencil_op(desc->stencil_back.depth_fail_op);
    state->back.reference = desc->stencil_back.reference;
    state->back.compare_mask = desc->stencil_read_mask;

    return (u32) state;
}

RHIBlendState vk_create_blend_state(RHIDevice device, RHIBlendDesc *desc) {
    auto state = (VulkanBlendState *) malloc(sizeof(VulkanBlendState));

    state->src_color_factor = vk_get_blend_factor(desc->src_color_factor);
    state->dst_color_factor = vk_get_blend_factor(desc->src_color_factor);
    state->color_op = vk_get_blend_op(desc->color_op);
    state->src_alpha_factor = vk_get_blend_factor(desc->src_alpha_factor);
    state->dst_alpha_factor = vk_get_blend_factor(desc->dst_alpha_factor);
    state->alpha_op = vk_get_blend_op(desc->alpha_op);
    state->color_write_mask = desc->color_write_mask;

    return (u64) state;
}

void vk_free_depth_stencil_state(RHIDevice device, RHIDepthStencilState state) {
    auto depth_stencil = (VulkanDepthStencilState *) state;

    VULKAN_VALIDATE(depth_stencil != nullptr, "vk_free_depth_stencil_state called with empty state", VULKAN_VALIDATION_SEVERITY_TRACE);

    if (depth_stencil == nullptr) {
        return;
    }

    free(depth_stencil);
}

void vk_free_blend_state(RHIDevice device, RHIBlendState state) {
    auto blend = (VulkanBlendState *) state;

    VULKAN_VALIDATE(blend != nullptr, "vk_free_blend_state called with empty state", VULKAN_VALIDATION_SEVERITY_TRACE);

    if (blend == nullptr) {
        return;
    }

    free(blend);
}

// Device
RHIDevice vk_create_device();

void vk_destroy_device(RHIDevice device);

void vk_device_wait_idle(RHIDevice device);

// Queue
RHIQueue vk_create_queue(RHIDevice device);

void vk_destroy_queue(RHIDevice device, RHIQueue queue);

RHICommandBuffer vk_start_command_recording(RHIQueue queue) {
    auto vulkan_queue = (VulkanQueue *) queue;

    VULKAN_VALIDATE(vulkan_queue != nullptr, "vk_start_command_recording called with null queue", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_queue == nullptr) {
        return 0;
    }

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vulkan_queue->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer cb = nullptr;
    if (vkAllocateCommandBuffers(vulkan_queue->device->device, &alloc_info, &cb) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_start_command_recording vkAllocateCommandBuffers failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return 0;
    }

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    if (vkBeginCommandBuffer(cb, &begin_info) != VK_SUCCESS) {
        vkFreeCommandBuffers(vulkan_queue->device->device, vulkan_queue->command_pool, 1, &cb);
        VULKAN_VALIDATE(false, "vk_start_command_recording vkBeginCommandBuffer failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return 0;
    }

    auto vkcb = (VulkanCommandBuffer *) malloc(sizeof(VulkanCommandBuffer));
    vkcb->command_buffer = cb;
    vkcb->queue = (VulkanQueue *) queue;

    return (u64) vkcb;
}

void vk_submit(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count, RHISemaphore sem, u64 sem_val) {
    auto vulkan_queue = (VulkanQueue *) queue;

    auto semaphore = (VulkanSemaphore *) sem;

    VULKAN_VALIDATE(vulkan_queue != nullptr, "vk_submit called with null queue", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(semaphore != nullptr, "vk_submit called with null semaphore", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_queue == nullptr || semaphore == nullptr) {
        return;
    }

    std::vector<VkSemaphoreSubmitInfo> signals;

    signals.push_back({
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore->semaphore,
        .value = sem_val,
        .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    });

    std::vector<VkSemaphoreSubmitInfo> waits;

    std::vector<VkCommandBufferSubmitInfo> cb_infos(command_buffer_count);

    for (u32 i = 0; i < cb_infos.size(); i++) {
        auto cb = (VulkanCommandBuffer*) command_buffers[i];
        VULKAN_VALIDATE(cb != nullptr, "vk_submit called with null command buffer", VULKAN_VALIDATION_SEVERITY_WARN);

        if (cb == nullptr) {
            continue;
        }

        VULKAN_VALIDATE(cb->queue == vulkan_queue, "vk_submit called with command buffer whose queue does not match the submission queue", VULKAN_VALIDATION_SEVERITY_WARN);

        if (cb->queue != vulkan_queue) {
            continue;
        }

        cb_infos[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        cb_infos[i].commandBuffer = cb->command_buffer;
        cb_infos[i].deviceMask = 0; // ???

        waits.insert(waits.end(), cb->waits.begin(), cb->waits.end());
        signals.insert(signals.end(), cb->signals.begin(), cb->signals.end());
    }

    VkSubmitInfo2 info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreInfoCount = (u32) waits.size(),
        .pWaitSemaphoreInfos = waits.data(),
        .commandBufferInfoCount = (u32) cb_infos.size(),
        .pCommandBufferInfos = cb_infos.data(),
        .signalSemaphoreInfoCount = (u32) signals.size(),
        .pSignalSemaphoreInfos = signals.data(),
    };
    
    if (vkQueueSubmit2(vulkan_queue->queue, 1, &info, nullptr) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_submit vkQueueSubmit2 failed", VULKAN_VALIDATION_SEVERITY_ERROR);
    }
}

// Semaphores
RHISemaphore vk_create_semaphore(RHIDevice device, u64 init_value) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return 0;
    }

    VkSemaphore sem = nullptr;

    VkSemaphoreTypeCreateInfo timeline_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = init_value
    };

    VkSemaphoreCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &timeline_info,
        .flags = 0,
    };

    if (vkCreateSemaphore(vulkan_device->device, &info, nullptr, &sem) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_create_semaphore vkCreateSemaphore failed", VULKAN_VALIDATION_SEVERITY_ERROR);
        return 0;
    }

    auto semaphore = (VulkanSemaphore *) malloc(sizeof(VulkanSemaphore));
    semaphore->semaphore = sem;

    return (u64) semaphore;
}

void vk_wait_semaphore(RHIDevice device, RHISemaphore sem, u64 value) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return;
    }

    auto semaphore = (VulkanSemaphore *) sem;

    VULKAN_VALIDATE(semaphore != nullptr, "vk_wait_semaphore called with null semaphore", VULKAN_VALIDATION_SEVERITY_WARN);

    if (semaphore == nullptr) {
        return;
    }

    VkSemaphoreWaitInfo info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &semaphore->semaphore,
        .pValues = &value
    };

    if (vkWaitSemaphores(vulkan_device->device, &info, UINT64_MAX) != VK_SUCCESS) {
        VULKAN_VALIDATE(false, "vk_wait_semaphore vkWaitSemaphores failed", VULKAN_VALIDATION_SEVERITY_ERROR);
    }
}

void vk_destroy_semaphore(RHIDevice device, RHISemaphore sem) {
    auto vulkan_device = (VulkanDevice *) device;

    VULKAN_VALIDATE(vulkan_device != nullptr, "insert_allocation_cpu called with null device", VULKAN_VALIDATION_SEVERITY_WARN);

    if (vulkan_device == nullptr) {
        return;
    }

    auto semaphore = (VulkanSemaphore *) sem;

    VULKAN_VALIDATE(semaphore != nullptr, "vk_destroy_semaphore called with null semaphore", VULKAN_VALIDATION_SEVERITY_TRACE);

    if (semaphore == nullptr) {
        return;
    }

    vkDestroySemaphore(vulkan_device->device, semaphore->semaphore, nullptr);
    free(semaphore);
}

// Commands
void vk_mem_copy(RHICommandBuffer cb, void *dst_gpu, void *src_gpu, u64 size) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VulkanBufferOffset dst = get_buffer_offset_gpu(command_buffer->queue->device, dst_gpu);
    VulkanBufferOffset src = get_buffer_offset_gpu(command_buffer->queue->device, src_gpu);

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_mem_copy called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, dst_gpu) != nullptr, "vk_mem_copy called with invalid dst", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, src_gpu) != nullptr, "vk_mem_copy called with invalid src", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (command_buffer == nullptr) {
        return;
    }

    VkBufferCopy copy = {
        .srcOffset = src.offset,
        .dstOffset = dst.offset,
        .size = size,
    };
    vkCmdCopyBuffer(command_buffer->command_buffer, src.buffer, dst.buffer, 1, &copy);
}

void vk_copy_to_texture(RHICommandBuffer cb, RHITexture texture, void *src_gpu) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    auto tex = (VulkanTexture *) texture;

    VulkanBufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, src_gpu);

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_copy_to_texture called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(tex != nullptr, "vk_copy_to_texture called with null tex", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, src_gpu) != nullptr, "vk_copy_to_texture called with invalid src", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (tex == nullptr || command_buffer == nullptr) {
        return;
    }

    VkBufferImageCopy copy = {
        .bufferOffset = offset.offset,
        .imageSubresource = {
            .aspectMask = tex->aspect_mask,
            .mipLevel = tex->mip_count,
            .baseArrayLayer = 0,
            .layerCount = tex->layers
        },
        .imageExtent = tex->extent
    };
    vkCmdCopyBufferToImage(command_buffer->command_buffer, offset.buffer, tex->image, tex->layout, 1, &copy);
}

void vk_copy_from_texture(RHICommandBuffer cb, void *dst_gpu, RHITexture texture) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    auto tex = (VulkanTexture *) texture;

    VulkanBufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, dst_gpu);

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_copy_from_texture called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(tex != nullptr, "vk_copy_from_texture called with null tex", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, dst_gpu) != nullptr, "vk_copy_from_texture called with invalid dst", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (tex == nullptr || command_buffer == nullptr) {
        return;
    }

    VkBufferImageCopy copy = {
        .bufferOffset = offset.offset,
        .imageSubresource = {
            .aspectMask = tex->aspect_mask,
            .mipLevel = tex->mip_count,
            .baseArrayLayer = 0,
            .layerCount = tex->layers
        },
        .imageExtent = tex->extent
    };
    vkCmdCopyImageToBuffer(command_buffer->command_buffer, tex->image, tex->layout, offset.buffer, 1, &copy);
}

void vk_set_active_texture_heap_ptr(RHICommandBuffer cb, void *ptr_gpu, u64 size) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_set_active_texture_heap_ptr called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr) {
        return;
    }

    VkBindHeapInfoEXT info = {
        .sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
        .heapRange = {
            .address = (VkDeviceAddress) ptr_gpu,
            .size = size,
        },
    };

    vkCmdBindSamplerHeapEXT(command_buffer->command_buffer, &info);
}

void vk_set_active_resource_heap_ptr(RHICommandBuffer cb, void *ptr_gpu, u64 size) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_set_active_resource_heap_ptr called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr) {
        return;
    }

    VkBindHeapInfoEXT info = {
        .sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
        .heapRange = {
            .address = (VkDeviceAddress) ptr_gpu,
            .size = size,
        },
    };

    vkCmdBindResourceHeapEXT(command_buffer->command_buffer, &info);
}

void vk_barrier(RHICommandBuffer cb, RHIPipelineStage before, RHIPipelineStage after, RHIHazardFlags hazards) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_mem_copy called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr) {
        return;
    }

    VkDependencyInfo dep = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO
    };

    VkMemoryBarrier2 mem = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .srcStageMask = vk_get_pipeline_stage(before),
        .srcAccessMask = 0,
        .dstStageMask = vk_get_pipeline_stage(after),
        .dstAccessMask = 0,
    };

    if (hazards & RHI_HAZARD_DRAW_ARGUMENTS) {
        mem.srcAccessMask |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        mem.dstStageMask |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    }

    if (hazards & RHI_HAZARD_DESCRIPTORS) {
        mem.srcAccessMask |= VK_ACCESS_SHADER_READ_BIT;
        mem.dstStageMask |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }

    if (hazards & RHI_HAZARD_DEPTH_STENCIL) {
        mem.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

        mem.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    }

    dep.memoryBarrierCount = 1;
    dep.pMemoryBarriers = &mem;

    vkCmdPipelineBarrier2(command_buffer->command_buffer, &dep);
}

void vk_signal_after(RHICommandBuffer cb, RHIPipelineStage before, RHISemaphore sem, u64 value) {
    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto semaphore = (VulkanSemaphore *) sem;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_signal_after called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(semaphore != nullptr, "vk_signal_after called with null semaphore", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr || semaphore == nullptr) {
        return;
    }

    command_buffer->signals.push_back({ 
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore->semaphore,
        .value = value,
        .stageMask = vk_get_pipeline_stage(before),
    });
}

void vk_wait_before(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value) {
    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto semaphore = (VulkanSemaphore *) sem;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_wait_before called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(semaphore != nullptr, "vk_wait_before called with null semaphore", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr || semaphore == nullptr) {
        return;
    }

    command_buffer->waits.push_back({
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore->semaphore,
        .value = value,
        .stageMask = vk_get_pipeline_stage(after),
    });
}

void vk_set_pipeline(RHICommandBuffer cb, RHIPipeline pipeline) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    auto p = (VulkanPipeline *) pipeline;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_set_pipeline called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(p != nullptr, "vk_set_pipeline called with null pipeline", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr || p == nullptr) {
        return;
    }

    vkCmdBindPipeline(command_buffer->command_buffer, p->bind_point, p->pipeline);
}

void vk_set_depth_stencil_state(RHICommandBuffer cb, RHIDepthStencilState state) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    auto depth_stencil = (VulkanDepthStencilState *) state;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_set_depth_stencil_state called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(depth_stencil != nullptr, "vk_set_depth_stencil_state called with null depth_stencil", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr || depth_stencil == nullptr) {
        return;
    }

    vkCmdSetDepthWriteEnable(command_buffer->command_buffer, depth_stencil->depth_write_enabled);
    vkCmdSetDepthCompareOp(command_buffer->command_buffer, depth_stencil->depth_compare_op);
    vkCmdSetDepthBiasEnable(command_buffer->command_buffer, true);
    vkCmdSetDepthBias(command_buffer->command_buffer, depth_stencil->depth_bias_constant, depth_stencil->depth_bias_clamp, depth_stencil->depth_bias_slope);

    vkCmdSetStencilWriteMask(command_buffer->command_buffer, VK_STENCIL_FACE_FRONT_BIT, depth_stencil->front.write_mask);
    vkCmdSetStencilOp(command_buffer->command_buffer, VK_STENCIL_FACE_FRONT_BIT, depth_stencil->front.fail_op, depth_stencil->front.pass_op, depth_stencil->front.depth_fail_op, depth_stencil->depth_compare_op);
    vkCmdSetStencilReference(command_buffer->command_buffer, VK_STENCIL_FACE_FRONT_BIT, depth_stencil->front.reference);
    vkCmdSetStencilCompareMask(command_buffer->command_buffer, VK_STENCIL_FACE_FRONT_BIT, depth_stencil->front.compare_mask);

    vkCmdSetStencilWriteMask(command_buffer->command_buffer, VK_STENCIL_FACE_BACK_BIT, depth_stencil->back.write_mask);
    vkCmdSetStencilOp(command_buffer->command_buffer, VK_STENCIL_FACE_BACK_BIT, depth_stencil->back.fail_op, depth_stencil->back.pass_op, depth_stencil->back.depth_fail_op, depth_stencil->depth_compare_op);
    vkCmdSetStencilReference(command_buffer->command_buffer, VK_STENCIL_FACE_BACK_BIT, depth_stencil->back.reference);
    vkCmdSetStencilCompareMask(command_buffer->command_buffer, VK_STENCIL_FACE_BACK_BIT, depth_stencil->back.compare_mask);
}

void vk_set_blend_state(RHICommandBuffer cb, RHIBlendState state) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    auto blend = (VulkanBlendState *) state;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_set_blend_state called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(blend != nullptr, "vk_set_blend_state called with null blend", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr || blend == nullptr) {
        return;
    }

    VkBool32 enable = true;
    vkCmdSetColorBlendEnableEXT(command_buffer->command_buffer, 0, 1, &enable);
    vkCmdSetColorWriteEnableEXT(command_buffer->command_buffer, 1, &enable);

    VkColorBlendEquationEXT eq = {
        .srcColorBlendFactor = blend->src_color_factor,
        .dstColorBlendFactor = blend->dst_color_factor,
        .colorBlendOp = blend->color_op,
        .srcAlphaBlendFactor = blend->src_alpha_factor,
        .dstAlphaBlendFactor = blend->dst_alpha_factor,
        .alphaBlendOp = blend->alpha_op
    };
    vkCmdSetColorBlendEquationEXT(command_buffer->command_buffer, 0, 1, &eq);

    vkCmdSetColorWriteMaskEXT(command_buffer->command_buffer, 0, 1, &blend->color_write_mask);
}

void vk_dispatch(RHICommandBuffer cb, void *data_gpu, u32 grid_dimensions[3]) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_dispatch called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, data_gpu) != nullptr, "vk_dispatch called with invalid gpu data", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (command_buffer == nullptr) {
        return;
    }

    VulkanComputePushConstants pc = {
        .data = data_gpu,
    };
    VkPushDataInfoEXT pd = {
        .sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
        .offset = 0,
        .data = {
            .address = &pc,
            .size = sizeof(pc)
        }
    };
    vkCmdPushDataEXT(command_buffer->command_buffer, &pd);

    vkCmdDispatch(command_buffer->command_buffer, grid_dimensions[0], grid_dimensions[1], grid_dimensions[2]);
}

void vk_dispatch_indirect(RHICommandBuffer cb, void *data_gpu, void *grid_dimensions_gpu) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_dispatch_indirect called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, data_gpu) != nullptr, "vk_dispatch_indirect called with invalid gpu data", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, grid_dimensions_gpu) != nullptr, "vk_dispatch_indirect called with invalid gpu grid dimensions", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (command_buffer == nullptr) {
        return;
    }

    VulkanComputePushConstants pc = {
        .data = data_gpu,
    };
    VkPushDataInfoEXT pd = {
        .sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
        .offset = 0,
        .data = {
            .address = &pc,
            .size = sizeof(pc)
        }
    };
    vkCmdPushDataEXT(command_buffer->command_buffer, &pd);

    VulkanBufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, grid_dimensions_gpu);
    vkCmdDispatchIndirect(command_buffer->command_buffer, offset.buffer, offset.offset);
}

void vk_begin_render_pass(RHICommandBuffer cb, RHIRenderPassDesc *desc) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_begin_render_pass called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr) {
        return;
    }

    VkRect2D render_area = {};
    render_area.extent.width = UINT32_MAX;
    render_area.extent.height = UINT32_MAX;

    std::vector<VkRenderingAttachmentInfo> color_attachments(desc->color_attachment_count);
    for (u32 i = 0; i < desc->color_attachment_count; i++) {
        auto texture = (VulkanTexture *) desc->color_attachments[i].texture;
        VULKAN_VALIDATE(texture != nullptr, "vk_begin_render_pass called with null texture in render pass description", VULKAN_VALIDATION_SEVERITY_WARN);
        if (texture == nullptr) {
            continue;
        }
        color_attachments[i] = VkRenderingAttachmentInfo {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = texture->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE
        };

        if (desc->color_attachments[i].clear) {
            color_attachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            color_attachments[i].clearValue.color.float32[0] = desc->color_attachments[i].clear_value.color[0];
            color_attachments[i].clearValue.color.float32[1] = desc->color_attachments[i].clear_value.color[1];
            color_attachments[i].clearValue.color.float32[2] = desc->color_attachments[i].clear_value.color[2];
            color_attachments[i].clearValue.color.float32[3] = desc->color_attachments[i].clear_value.color[3];
        }
        
        if (texture->extent.width < render_area.extent.width) {
            render_area.extent.width = texture->extent.width;
        }
        if (texture->extent.height < render_area.extent.height) {
            render_area.extent.height = texture->extent.height;
        }
    }

    VkRenderingInfo info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = render_area,
        .layerCount = 1,
        .colorAttachmentCount = (u32) color_attachments.size(),
        .pColorAttachments = color_attachments.data(),
    };

    auto depth_stencil = (VulkanTexture *) desc->depth_stencil_attachment.texture;
    
    VkRenderingAttachmentInfo depth_stencil_attachment;
    if (depth_stencil) {
        if (depth_stencil->extent.width < render_area.extent.width) {
            render_area.extent.width = depth_stencil->extent.width;
        }
        if (depth_stencil->extent.height < render_area.extent.height) {
            render_area.extent.height = depth_stencil->extent.height;
        }

        depth_stencil_attachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = depth_stencil->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE
        };

        if (desc->depth_stencil_attachment.clear) {
            depth_stencil_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depth_stencil_attachment.clearValue.depthStencil.depth = desc->depth_stencil_attachment.clear_value.depth_stencil.depth;
            depth_stencil_attachment.clearValue.depthStencil.stencil = desc->depth_stencil_attachment.clear_value.depth_stencil.stencil;
        }

        info.pDepthAttachment = &depth_stencil_attachment;
        info.pStencilAttachment = &depth_stencil_attachment;
    }

    vkCmdBeginRendering(command_buffer->command_buffer, &info);
}

void vk_end_render_pass(RHICommandBuffer cb) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_end_render_pass called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);

    if (command_buffer == nullptr) {
        return;
    }

    vkCmdEndRendering(command_buffer->command_buffer);
}

void vk_draw_indexed_instanced(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_draw_indexed_instanced called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, vertex_data_gpu) != nullptr, "vk_draw_indexed_instanced called with invalid vertex_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu) != nullptr, "vk_draw_indexed_instanced called with invalid pixel_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, indices_gpu) != nullptr, "vk_draw_indexed_instanced called with invalid indices_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (command_buffer == nullptr) {
        return;
    }

    VulkanRasterPushConstants pc = {
        .vert_data = vertex_data_gpu,
        .frag_data = pixel_data_gpu
    };
    VkPushDataInfoEXT pd = {
        .sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
        .offset = 0,
        .data = {
            .address = &pc,
            .size = sizeof(pc)
        }
    };
    vkCmdPushDataEXT(command_buffer->command_buffer, &pd);

    VulkanBufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, indices_gpu);
    vkCmdBindIndexBuffer(command_buffer->command_buffer, offset.buffer, offset.offset, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(command_buffer->command_buffer, index_count, instance_count, 0, 0, 0);
}

void vk_draw_indexed_instanced_indirect(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, void *args_gpu) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_draw_indexed_instanced_indirect called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, vertex_data_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid vertex_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid pixel_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, indices_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid indices_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, args_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid args_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    
    if (command_buffer == nullptr) {
        return;
    }

    VulkanRasterPushConstants pc = {
        .vert_data = vertex_data_gpu,
        .frag_data = pixel_data_gpu,
    };
    VkPushDataInfoEXT pd = {
        .sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
        .offset = 0,
        .data = {
            .address = &pc,
            .size = sizeof(pc)
        }
    };
    vkCmdPushDataEXT(command_buffer->command_buffer, &pd);

    VulkanBufferOffset index_offset = get_buffer_offset_gpu(command_buffer->queue->device, indices_gpu);
    vkCmdBindIndexBuffer(command_buffer->command_buffer, index_offset.buffer, index_offset.offset, VK_INDEX_TYPE_UINT16);

    VulkanBufferOffset indirect_offset = get_buffer_offset_gpu(command_buffer->queue->device, args_gpu);
    vkCmdDrawIndexedIndirect(command_buffer->command_buffer, indirect_offset.buffer, indirect_offset.offset, 1, 0);
}

void vk_draw_indexed_instanced_indirect_multi(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *args_gpu, void *draw_count_gpu, u32 stride) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_draw_indexed_instanced_indirect called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, vertex_data_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid vertex_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid pixel_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, draw_count_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid draw_count_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, args_gpu) != nullptr, "vk_draw_indexed_instanced_indirect called with invalid args_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (command_buffer == nullptr) {
        return;
    }

    VulkanRasterPushConstants pc = {
        .vert_data = vertex_data_gpu,
        .frag_data = pixel_data_gpu,
    };
    VkPushDataInfoEXT pd = {
        .sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
        .offset = 0,
        .data = {
            .address = &pc,
            .size = sizeof(pc)
        }
    };
    vkCmdPushDataEXT(command_buffer->command_buffer, &pd);

    VulkanBufferOffset indirect_offset = get_buffer_offset_gpu(command_buffer->queue->device, args_gpu);
    VulkanBufferOffset count_offset = get_buffer_offset_gpu(command_buffer->queue->device, draw_count_gpu);
    vkCmdDrawIndexedIndirectCount(command_buffer->command_buffer, indirect_offset.buffer, indirect_offset.offset, count_offset.buffer, count_offset.offset, 0, stride);
}

void vk_draw_meshlets(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, u32 dim[3]) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_draw_meshlets called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, meshlet_data_gpu) != nullptr, "vk_draw_meshlets called with invalid vertex_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu) != nullptr, "vk_draw_meshlets called with invalid pixel_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (command_buffer == nullptr) {
        return;
    }

    VulkanMeshPushConstants pc = {
        .mesh_data = meshlet_data_gpu,
        .frag_data = pixel_data_gpu
    };
    VkPushDataInfoEXT pd = {
        .sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
        .offset = 0,
        .data = {
            .address = &pc,
            .size = sizeof(pc)
        }
    };
    vkCmdPushDataEXT(command_buffer->command_buffer, &pd);

    vkCmdDrawMeshTasksEXT(command_buffer->command_buffer, dim[0], dim[1], dim[2]);
}

void vk_draw_meshlets_indirect(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, void *dim_gpu) {
    auto command_buffer = (VulkanCommandBuffer *) cb;

    VULKAN_VALIDATE(command_buffer != nullptr, "vk_draw_meshlets_indirect called with null command_buffer", VULKAN_VALIDATION_SEVERITY_WARN);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, meshlet_data_gpu) != nullptr, "vk_draw_meshlets_indirect called with invalid vertex_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu) != nullptr, "vk_draw_meshlets_indirect called with invalid pixel_data_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);
    VULKAN_VALIDATE(find_allocation_gpu(command_buffer->queue->device, dim_gpu) != nullptr, "vk_draw_meshlets_indirect called with invalid dim_gpu", VULKAN_VALIDATION_SEVERITY_CRITICAL);

    if (command_buffer == nullptr) {
        return;
    }

    VulkanMeshPushConstants pc = {
        .mesh_data = meshlet_data_gpu,
        .frag_data = pixel_data_gpu
    };
    VkPushDataInfoEXT pd = {
        .sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
        .offset = 0,
        .data = {
            .address = &pc,
            .size = sizeof(pc)
        }
    };
    vkCmdPushDataEXT(command_buffer->command_buffer, &pd);

    VulkanBufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, dim_gpu);
    vkCmdDrawMeshTasksIndirectEXT(command_buffer->command_buffer, offset.buffer, offset.offset, 1, 0);
}