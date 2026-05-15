#include "VulkanRHI.h"

#include "Log.h"

#include <vulkan/vulkan.h>

#include "VulkanPlatform.h"

#ifdef SK_DEBUG
static const bool enable_validation = true;
const char *validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};
const u32 validation_layer_count = 1;
const char *device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
    VK_EXT_MESH_SHADER_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME,
};
const u32 device_extension_count = 2;

// TODO:
#define ASSERT_RESOURCE_GPU(device, ptr) assert((ptr) == NULL || find_allocation_gpu((device), (ptr)))
#define ASSERT_RESOURCE_CPU(device, ptr) assert((ptr) == NULL || find_allocation_cpu((device), (ptr)))
#define ASSERT_RESOURCE_GPU_STRICT(device, ptr) assert((ptr) && find_allocation_gpu((device), (ptr)))
#define ASSERT_RESOURCE_CPU_STRICT(device, ptr) assert((ptr) && find_allocation_cpu((device), (ptr)))

#define ASSERT_RESOURCE_IS(device, ptr, Type) ASSERT_RESOURCE_##Type##(device, ptr)
#define ASSERT_RESOURCE_IS_STRICT(device, ptr, Type) ASSERT_RESOURCE_##Type##_STRICT(device, ptr)
#endif

struct VulkanQueue;
struct VulkanTexture;

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
    VkDevice device;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkQueue queue;

    VkFence fence;

    u32 image_count;
    VulkanTexture *textures;
    VkExtent2D extent;

    u32 semaphore_index;
    VkSemaphore *acquire_semaphores;
    VkSemaphore *present_semaphores;
};

struct VulkanQueueFamilyAssignment {
    u32 family;
    u32 index;
    u32 capabilities;
};

struct VulkanQueueFamilyInfo {
    u32 total_count;
    u32 used_count;

    VkQueueFlags flags;
    bool supports_present;
};

struct VulkanDevice {
    VkDevice device;
    VkPhysicalDevice gpu;
    VkCommandPool command_pool;

    std::vector<AllocBlock> cpu_allocations;
    std::vector<AllocBlock> gpu_allocations;

    VulkanQueue *queues;
    u32 queue_count;
};

struct VulkanSemaphore {
    VkSemaphore semaphore;
};

struct VulkanQueue {
    VkQueue queue;

    u32 family;
    u32 index;
    u32 capabilities;

    struct VulkanDevice *device;

    VkCommandPool command_pool;
};

struct VulkanBackbufferData {
    VkSemaphore present_semaphore;
    VkSemaphore acquire_semaphore;
    VkSemaphore timeline;
    bool acquire_consumed;
    u64 ready_value;
    u64 present_value;

    u32 image_index;
};

struct VulkanTexture {
    VkImage image;
    VkImageView image_view; // NOTE: Only used for setting as a render target, not used for descriptors
    VkFormat format;
    VkExtent3D extent;
    u32 layers;
    u32 mip_count;
    u32 sample_count;
    VkImageUsageFlags usage;
    VkImageType type;
    VkImageAspectFlags aspect_mask;

    VulkanBackbufferData *backbuffer_data; // only populated for swapchain images
};

struct VulkanCommandBuffer {
    VkCommandBuffer command_buffer;
    VulkanQueue *queue;
    std::vector<VkSemaphoreSubmitInfo> waits;
    std::vector<VkSemaphoreSubmitInfo> signals;

    VulkanTexture *backbuffer; // swapchain backbuffer if one is used
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

static Vulkan vulkan;

static VkImageViewType get_view_type(VkImageType type) {
    switch (type) {
        case VK_IMAGE_TYPE_1D:
            return VK_IMAGE_VIEW_TYPE_1D;
        case VK_IMAGE_TYPE_2D:
            return VK_IMAGE_VIEW_TYPE_2D;
        case VK_IMAGE_TYPE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
        default:
            assert(false);
    }
}

static VkFormat get_format(RHIFormat format) {
    switch (format) {
        case RHI_FORMAT_NONE:
            return VK_FORMAT_UNDEFINED;
        case RHI_FORMAT_RGBA8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case RHI_FORMAT_D32_FLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case RHI_FORMAT_RGB10_A2_UNORM:
            return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case RHI_FORMAT_RGBA8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        default:
            assert(false);
    }
}

static VkImageAspectFlags get_aspect_mask(RHIFormat format) {
    switch (format) {
        case RHI_FORMAT_NONE:
            return VK_IMAGE_ASPECT_NONE;
        case RHI_FORMAT_RGB10_A2_UNORM:
        case RHI_FORMAT_RGBA8_UNORM:
        case RHI_FORMAT_RGBA8_SRGB:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case RHI_FORMAT_D32_FLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        default:
            assert(false);
    }
}

static VkResult create_debug_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *info, const VkAllocationCallbacks *allocator, VkDebugUtilsMessengerEXT *messenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, info, allocator, messenger);
    }
    SK_LOG_ERROR("Extension not present");
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void destroy_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks *allocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, messenger, allocator);
    }
}

static VkStencilOp vk_get_stencil_op(RHIStencilOp op) {
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
            assert(false);
    }
}

static VkCompareOp vk_get_compare_op(RHIOp op) {
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
            assert(false);
    }
}

static VkPipelineStageFlags2 vk_get_pipeline_stage(RHIPipelineStage stage) {
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
            assert(false);
    }
}

static VkPrimitiveTopology vk_get_topology(RHITopology t) {
    switch (t) {
        case RHI_TOPOLOGY_TRIANGLE_FAN:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case RHI_TOPOLOGY_TRIANGLE_LIST:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case RHI_TOPOLOGY_TRIANGLE_STRIP:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        default:
            assert(false);
    }
}

static VkBlendFactor vk_get_blend_factor(RHIBlendFactor factor) {
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
            assert(false);
    }
}

static VkBlendOp vk_get_blend_op(RHIBlendOp op) {
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
            assert(false);
    }
}

static VkCullModeFlags vk_get_cull_mode(RHICullMode mode) {
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
            assert(false);
    }
}

static AllocBlock *find_allocation_gpu(VulkanDevice *device, void *gpu) {
    assert(device);
    assert(gpu);

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

static AllocBlock *find_allocation_cpu(VulkanDevice *device, void *cpu) {
    assert(device);
    assert(cpu);

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

static VulkanBufferOffset get_buffer_offset_gpu(VulkanDevice *device, void *gpu) {
    assert(device);
    assert(gpu);

    AllocBlock *block = find_allocation_gpu(device, gpu);

    assert(block);

    u64 offset = (u64) gpu - (u64) block->gpu;

    return { block->buffer, offset };
}

static bool find_memory_type(VkPhysicalDevice gpu, u32 *memory_type_index, u32 filter, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(gpu, &mem_props);

    for (auto i = 0; i < mem_props.memoryTypeCount; i++) {
        if (((filter & (1 << i)) != 0) && ((mem_props.memoryTypes[i].propertyFlags & flags) == flags)) {
            *memory_type_index = i;
            return true;
        }
    }

    assert(false);

    return false;
}

static u64 find_insert_pos_cpu(VulkanDevice *device, void *cpu) {
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

static u64 find_insert_pos_gpu(VulkanDevice *device, void *gpu) {
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

static void insert_allocation_cpu(VulkanDevice *device, AllocBlock in) {
    assert(device);

    auto pos = find_insert_pos_cpu(device, in.cpu);
    device->cpu_allocations.insert(device->cpu_allocations.begin() + pos, in);
}

static void insert_allocation_gpu(VulkanDevice *device, AllocBlock in) {
    assert(device);

    auto pos = find_insert_pos_gpu(device, in.gpu);
    device->gpu_allocations.insert(device->gpu_allocations.begin() + pos, in);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type, 
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data, 
    void *user_data) 
{
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

static bool queue_family_supports(VulkanQueueFamilyInfo *family, u32 capabilities) {
    if ((capabilities & RHI_QUEUE_GRAPHICS) && !(family->flags & VK_QUEUE_GRAPHICS_BIT)) {
        return false;
    }

    if ((capabilities & RHI_QUEUE_COMPUTE) && !(family->flags & VK_QUEUE_COMPUTE_BIT)) {
        return false;
    }

    if ((capabilities & RHI_QUEUE_TRANSFER) && !(family->flags & VK_QUEUE_TRANSFER_BIT)) {
        return false;
    }

    return true;
}

// Memory
void *vk_alloc(RHIDevice device, u64 bytes, RHIMemoryType memory = RHI_MEMORY_TYPE_DEFAULT) {
    assert(device);
    assert(bytes);

    auto vulkan_device = (VulkanDevice *) device;

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
        assert(false);
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
            assert(false);
    }

    if (!find_memory_type(vulkan_device->gpu, &alloc_info.memoryTypeIndex, mem_reqs.memoryTypeBits, properties) != VK_SUCCESS) {
        vkDestroyBuffer(vulkan_device->device, alloc_block.buffer, nullptr);
        assert(false);
    }

    if (vkAllocateMemory(vulkan_device->device, &alloc_info, nullptr, &alloc_block.memory) != VK_SUCCESS) {
        vkDestroyBuffer(vulkan_device->device, alloc_block.buffer, nullptr);
        assert(false);
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
            assert(false);
        }

        insert_allocation_cpu(vulkan_device, alloc_block);
        insert_allocation_gpu(vulkan_device, alloc_block);
        return alloc_block.cpu;
    }

    insert_allocation_gpu(vulkan_device, alloc_block);
    return alloc_block.gpu;
}

void vk_free(RHIDevice device, void *ptr) {
    assert(device);
    assert(ptr);
    auto vulkan_device = (VulkanDevice *) device;

    AllocBlock *block = find_allocation_cpu(vulkan_device, ptr);
    if (!block) {
        block = find_allocation_gpu(vulkan_device, ptr);
    }

    assert(block);

    if (block->cpu) {
        vkUnmapMemory(vulkan_device->device, block->memory);
    }
    vkFreeMemory(vulkan_device->device, block->memory, nullptr);
    vkDestroyBuffer(vulkan_device->device, block->buffer, nullptr);
}

void *vk_host_to_device_pointer(RHIDevice device, void *ptr) {
    assert(device);
    assert(ptr);

    auto vulkan_device = (VulkanDevice *) device;

    AllocBlock *block = find_allocation_cpu(vulkan_device, ptr);

    assert(block);

    u64 diff = (u64) ptr - (u64) block->cpu;

    return (void *) ((u64) block->gpu + (u64) diff);
}

// Textures
RHITextureSizeAlign vk_texture_size_align(RHIDevice device, RHITextureDesc *desc);

RHITexture vk_create_texture(RHIDevice device, RHITextureDesc *desc, void *ptr_gpu);

void vk_destroy_texture(RHIDevice device, RHITexture texture);

RHITextureDescriptor vk_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc) {
    assert(device);
    assert(texture);
    assert(desc);

    auto vulkan_device = (VulkanDevice *) device;
    auto vulkan_texture = (VulkanTexture *) texture;

    RHITextureDescriptor descriptor = {};

    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = vulkan_texture->image,
        .viewType = get_view_type(vulkan_texture->type),
        .format = get_format(desc->format),
        .subresourceRange = {
            .aspectMask = get_aspect_mask(desc->format),
            .baseMipLevel = desc->base_mip,
            .levelCount = desc->mip_count,
            .baseArrayLayer = desc->base_layer,
            .layerCount = desc->layer_count
        }
    };

    VkImageDescriptorInfoEXT image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_INFO_EXT,
        .pView = &view_info,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkHostAddressRangeEXT host_address_range = {
        .address = &descriptor,
        .size = sizeof(descriptor)
    };

    VkResourceDescriptorInfoEXT resource_info = {
        .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
        .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .data = &image_info
    };

    if (vkWriteResourceDescriptorsEXT(vulkan_device->device, 1, &resource_info, &host_address_range) != VK_SUCCESS) {
        assert(false);
    }

    return descriptor;
}

RHITextureDescriptor vk_rw_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc) {
    assert(device);
    assert(texture);
    assert(desc);

    auto vulkan_device = (VulkanDevice *) device;
    auto vulkan_texture = (VulkanTexture *) texture;

    RHITextureDescriptor descriptor = {};

    VkImageViewCreateInfo view_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = vulkan_texture->image,
        .viewType = get_view_type(vulkan_texture->type),
        .format = get_format(desc->format),
        .subresourceRange = {
            .aspectMask = get_aspect_mask(desc->format),
            .baseMipLevel = desc->base_mip,
            .levelCount = desc->mip_count,
            .baseArrayLayer = desc->base_layer,
            .layerCount = desc->layer_count
        }
    };

    VkImageDescriptorInfoEXT image_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_INFO_EXT,
        .pView = &view_info,
        .layout = VK_IMAGE_LAYOUT_GENERAL
    };

    VkHostAddressRangeEXT host_address_range = {
        .address = &descriptor,
        .size = sizeof(descriptor)
    };

    VkResourceDescriptorInfoEXT resource_info = {
        .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
        .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .data = &image_info
    };

    if (vkWriteResourceDescriptorsEXT(vulkan_device->device, 1, &resource_info, &host_address_range) != VK_SUCCESS) {
        assert(false);
    }

    return descriptor;
}

// Pipelines
RHIPipeline vk_create_compute_pipeline(RHIDevice device, u8 *compute_ir, u32 ir_size) {
    assert(device);
    assert(compute_ir);
    assert(ir_size);

    auto vulkan_device = (VulkanDevice *) device;

    VkShaderModuleCreateInfo shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = ir_size,
        .pCode = (u32 *) compute_ir,
    };

    VkShaderModule shader_module = nullptr;

    if (vkCreateShaderModule(vulkan_device->device, &shader_info, nullptr, &shader_module) != VK_SUCCESS) {
        assert(false);
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
        assert(false);
    }

    vkDestroyShaderModule(vulkan_device->device, shader_module, nullptr);

    auto pipeline = (VulkanPipeline *) malloc(sizeof(VulkanPipeline));
    if (!pipeline) {
        vkDestroyPipeline(vulkan_device->device, vulkan_pipeline, nullptr);

        assert(false);
    }
    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
    return (u64) pipeline;
}

RHIPipeline vk_create_graphics_pipeline(RHIDevice device, u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    assert(device);
    assert(vertex_ir);
    assert(vertex_ir_size);
    assert(pixel_ir);
    assert(pixel_ir_size);
    assert(desc);

    auto vulkan_device = (VulkanDevice *) device;

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
        assert(false);
    }

    if (vkCreateShaderModule(vulkan_device->device, &vertex_shader_info, nullptr, &fragment_shader_module) != VK_SUCCESS) {
        vkDestroyShaderModule(vulkan_device->device, vertex_shader_module, nullptr);

        assert(false);
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

        assert(false);
    }

    vkDestroyShaderModule(vulkan_device->device, vertex_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

    auto pipeline = (VulkanPipeline *) malloc(sizeof(VulkanPipeline));
    if (!pipeline) {
        vkDestroyPipeline(vulkan_device->device, vulkan_pipeline, nullptr);

        assert(false);
    }
    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return (u64) pipeline;
}

RHIPipeline vk_create_graphics_meshlet_pipeline(RHIDevice device, u8 *meshlet_ir, u32 meshlet_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    assert(device);
    assert(meshlet_ir);
    assert(meshlet_ir_size);
    assert(pixel_ir);
    assert(pixel_ir_size);
    assert(desc);


    auto vulkan_device = (VulkanDevice *) device;

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
        assert(false);
    }

    if (vkCreateShaderModule(vulkan_device->device, &fragment_shader_info, nullptr, &fragment_shader_module) != VK_SUCCESS) {
        vkDestroyShaderModule(vulkan_device->device, mesh_shader_module, nullptr);

        assert(false);
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

    VkPipeline vulkan_pipeline = nullptr;
    if (vkCreateGraphicsPipelines(vulkan_device->device, nullptr, 1, &info, nullptr, &vulkan_pipeline) != VK_SUCCESS) {
        vkDestroyShaderModule(vulkan_device->device, mesh_shader_module, nullptr);
        vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

        assert(false);
    }

    vkDestroyShaderModule(vulkan_device->device, mesh_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

    auto pipeline = (VulkanPipeline *) malloc(sizeof(VulkanPipeline));
    if (!pipeline) {
        vkDestroyPipeline(vulkan_device->device, vulkan_pipeline, nullptr);

        assert(false);
    }
    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return (u64) pipeline;
}

void vk_destroy_pipeline(RHIDevice device, RHIPipeline pipeline) {
    assert(device);
    assert(pipeline);

    auto vulkan_device = (VulkanDevice *) device;
    auto vulkan_pipeline = (VulkanPipeline *) pipeline;

    vkDestroyPipeline(vulkan_device->device, vulkan_pipeline->pipeline, nullptr);
    free(vulkan_pipeline);
}

// State objects
RHIDepthStencilState vk_create_depth_stencil_state(RHIDevice device, RHIDepthStencilDesc *desc) {
    assert(device);
    assert(desc);

    auto state = (VulkanDepthStencilState *) malloc(sizeof(VulkanDepthStencilState));

    assert(state);

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
    assert(device);
    assert(desc);

    auto state = (VulkanBlendState *) malloc(sizeof(VulkanBlendState));

    assert(state);

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
    assert(device);
    assert(state);

    auto depth_stencil = (VulkanDepthStencilState *) state;

    free(depth_stencil);
}

void vk_free_blend_state(RHIDevice device, RHIBlendState state) {
    assert(device);
    assert(state);

    auto blend = (VulkanBlendState *) state;

    free(blend);
}

// Device
RHIDevice vk_create_device(RHIDeviceDesc *desc) {
    u32 gpu_count = 0;
    if (vkEnumeratePhysicalDevices(vulkan.instance, &gpu_count, nullptr) != VK_SUCCESS) {
        assert(false);
    }
    auto gpus = (VkPhysicalDevice *) malloc(sizeof(VkPhysicalDevice) * gpu_count);
    if (vkEnumeratePhysicalDevices(vulkan.instance, &gpu_count, gpus) != VK_SUCCESS) {
        free(gpus);

        assert(false);
    }

    VkPhysicalDevice gpu = nullptr;

    VulkanQueueFamilyAssignment *selected_queue_assignments = nullptr;
    u32 selected_queue_assignment_count = 0;

    VulkanQueueFamilyInfo *selected_family_infos = nullptr;
    u32 selected_family_count = 0;

    for (u32 i = 0; i < gpu_count; i++) {
        u32 extension_count = 0;

        if (vkEnumerateDeviceExtensionProperties(gpus[i], nullptr, &extension_count, nullptr) != VK_SUCCESS) {
            free(gpus);

            assert(false);
        }
        auto extensions = (VkExtensionProperties *) malloc(sizeof(VkExtensionProperties) * extension_count);
        if (vkEnumerateDeviceExtensionProperties(gpus[i], nullptr, &extension_count, extensions) != VK_SUCCESS) {
            free(gpus);
            free(extensions);

            assert(false);
        }

        bool extensions_supported = true;

        for (u32 k = 0; k < device_extension_count; k++) {
            bool extension_supported = false;
            for (u32 j = 0; j < extension_count; j++) {
                if (strcmp(device_extensions[k], extensions[j].extensionName) == 0) {
                    extension_supported = true;
                }
            }

            if (!extension_supported) {
                extensions_supported = false;
                break;
            }
        }

        free(extensions);

        VkPhysicalDeviceVulkan14Features features14 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        };
        VkPhysicalDeviceVulkan13Features features13 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = &features14
        };
        VkPhysicalDeviceVulkan12Features features12 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .pNext = &features13
        };
        VkPhysicalDeviceVulkan11Features features11 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
            .pNext = &features12
        };
        VkPhysicalDeviceFeatures2 features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &features11
        };

        vkGetPhysicalDeviceFeatures2(gpus[i], &features);

        u32 family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &family_count, nullptr);

        auto families = (VkQueueFamilyProperties *) malloc(sizeof(VkQueueFamilyProperties) * family_count);
        if (!families) {
            free(gpus);

            assert(false);
        }

        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &family_count, families);

        auto family_infos = (VulkanQueueFamilyInfo *) malloc(sizeof(VulkanQueueFamilyInfo) * family_count);
        if (!family_infos) {
            free(gpus);
            free(families);

            assert(false);
        }

        for (u32 j = 0; j < family_count; j++) {
            family_infos[j].total_count = families[j].queueCount;

            family_infos[j].used_count = 0;
            family_infos[j].flags = families[j].queueFlags;

            family_infos[j].supports_present = false;
        }

        free(families);

        auto queue_assignments = (VulkanQueueFamilyAssignment *) malloc(sizeof(VulkanQueueFamilyAssignment) * 64);
        if (!queue_assignments) {
            free(gpus);
            free(family_infos);

            assert(false);
        }

        u32 assignment_count = 0;

        bool queues_supported = true;

        for (u32 j = 0; j < desc->queue_count; j++) {
            RHIQueueRequest *request = &desc->queues[j];

            for (u32 k = 0; k < request->count; k++) {

                bool found = false;

                for (u32 family = 0;
                    family < family_count;
                    family++) {
                    VulkanQueueFamilyInfo *info = &family_infos[family];

                    if (!queue_family_supports(info, request->capabilities)) {
                        continue;
                    }

                    if (info->used_count >= info->total_count) {
                        continue;
                    }

                    VulkanQueueFamilyAssignment assignment = {};
                    assignment.family = family;
                    assignment.index = info->used_count;
                    assignment.capabilities = request->capabilities;

                    queue_assignments[assignment_count++] = assignment;

                    info->used_count++;

                    found = true;
                    break;
                }

                if (!found) {
                    queues_supported = false;
                    break;
                }
            }

            if (!queues_supported) {
                break;
            }
        }

        if (extensions_supported &&
            features.features.fullDrawIndexUint32 &&
            features.features.drawIndirectFirstInstance &&
            features.features.depthClamp &&
            features.features.depthBiasClamp &&
            features.features.samplerAnisotropy &&
            features12.drawIndirectCount &&
            features12.timelineSemaphore &&
            features12.bufferDeviceAddress &&
            features13.synchronization2 &&
            features13.dynamicRendering &&
            features14.maintenance5 &&
            queues_supported) 
        {
            gpu = gpus[i];

            selected_queue_assignments = queue_assignments;
            selected_queue_assignment_count = assignment_count;

            selected_family_infos = family_infos;
            selected_family_count = family_count;
            break;
        }
    }

    free(gpus);

    if (!gpu) {
        free(selected_queue_assignments);
        free(selected_family_infos);

        assert(false);
    }

    VkPhysicalDeviceVulkan14Features features14 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .maintenance5 = true,
    };
    VkPhysicalDeviceVulkan13Features features13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &features14,
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    VkPhysicalDeviceVulkan12Features features12 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = &features13,
        .drawIndirectCount = true,
        .timelineSemaphore = true,
        .bufferDeviceAddress = true,
    };
    VkPhysicalDeviceVulkan11Features features11 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
        .pNext = &features12,
    };
    VkPhysicalDeviceFeatures2 features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &features11,
        .features = {
            .fullDrawIndexUint32 = true,
            .drawIndirectFirstInstance = true,
            .depthClamp = true,
            .depthBiasClamp = true,
            .samplerAnisotropy = true
        }
    };

    // TODO: Max queues per family
    auto priorities = (float *) malloc(sizeof(float) * 64);

    if (!priorities) {
        free(selected_queue_assignments);
        free(selected_family_infos);

        assert(false);
    }

    for (u32 i = 0; i < 64; i++) {
        priorities[i] = 1.0f;
    }

    auto queue_infos = (VkDeviceQueueCreateInfo *) malloc(sizeof(VkDeviceQueueCreateInfo) * selected_family_count);

    if (!queue_infos) {
        free(priorities);
        free(selected_queue_assignments);
        free(selected_family_infos);

        assert(false);
    }

    u32 queue_info_count = 0;

    for (u32 i = 0; i < selected_family_count; i++) {

        VulkanQueueFamilyInfo *family = &selected_family_infos[i];

        if (family->used_count == 0) {
            continue;
        }

        VkDeviceQueueCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = i,
            .queueCount = family->used_count,
            .pQueuePriorities = priorities
        };

        queue_infos[queue_info_count++] = info;
    }

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,

        .queueCreateInfoCount = queue_info_count,
        .pQueueCreateInfos = queue_infos,

        .enabledExtensionCount = device_extension_count,
        .ppEnabledExtensionNames = device_extensions,
    };

    VkDevice vk_device = nullptr;

    if (vkCreateDevice(gpu, &create_info, nullptr, &vk_device) != VK_SUCCESS) {
        free(priorities);
        free(queue_infos);
        free(selected_queue_assignments);
        free(selected_family_infos);

        assert(false);
    }

    free(priorities);
    free(queue_infos);

    auto device = (VulkanDevice *) malloc(sizeof(VulkanDevice));
    if (!device) {
        free(selected_queue_assignments);
        free(selected_family_infos);

        assert(false);
    }

    device->gpu_allocations = {};
    device->cpu_allocations = {};

    device->device = vk_device;
    device->gpu = gpu;

    device->queues = (VulkanQueue *) malloc(sizeof(VulkanQueue) * selected_queue_assignment_count);
    if (!device->queues) {
        vkDestroyDevice(vk_device, nullptr);
        free(selected_queue_assignments);
        free(selected_family_infos);

        assert(false);
    }
    device->queue_count = selected_queue_assignment_count;

    for (u32 i = 0; i < selected_queue_assignment_count; i++) {
        VulkanQueueFamilyAssignment *assignment = &selected_queue_assignments[i];

        VulkanQueue *queue = &device->queues[i];
        memset(queue, 0, sizeof(VulkanQueue));

        queue->family = assignment->family;
        queue->index = assignment->index;
        queue->capabilities = assignment->capabilities;

        queue->device = device;

        vkGetDeviceQueue(device->device, assignment->family, assignment->index, &queue->queue);

        VkCommandPoolCreateInfo pool_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // TODO: Is there a good way to get rid of this?
            .queueFamilyIndex = assignment->family
        };

        if (vkCreateCommandPool(device->device, &pool_info, nullptr, &queue->command_pool) != VK_SUCCESS) {
            for (u32 j = 0; j < i; j++) {
                vkDestroyCommandPool(vk_device, device->queues[j].command_pool, nullptr);
            }
            vkDestroyDevice(vk_device, nullptr);
            free(selected_queue_assignments);
            free(selected_family_infos);
            free(device->queues);
            free(device);

            assert(false);
        }
    }

    free(selected_queue_assignments);
    free(selected_family_infos);

    return (RHIDevice) device;
}

void vk_destroy_device(RHIDevice device) {
    assert(device);

    auto vulkan_device = (VulkanDevice *) device;

    for (u32 i = 0; i < vulkan_device->queue_count; i++) {
        vkDestroyCommandPool(vulkan_device->device, vulkan_device->queues[i].command_pool, nullptr);
    }
    free(vulkan_device->queues);

    vulkan_device->cpu_allocations.~vector();
    vulkan_device->gpu_allocations.~vector();

    vkDestroyDevice(vulkan_device->device, nullptr);

    free(vulkan_device);
}

void vk_device_wait_idle(RHIDevice device) {
    assert(device);

    auto vulkan_device = (VulkanDevice *) device;

    vkDeviceWaitIdle(vulkan_device->device);
}

// Queue
RHIQueue vk_get_queue(RHIDevice device, RHIQueueDesc *desc) {
    assert(device);
    assert(desc);

    auto vulkan_device = (VulkanDevice *) device;

    u32 found = 0;

    for (u32 i = 0; i < vulkan_device->queue_count; i++) {
        if ((vulkan_device->queues[i].capabilities & desc->capabilities) != desc->capabilities) {
            continue;
        }

        if (found == desc->index) {
            return (u32)  &vulkan_device->queues[i];
        }

        found++;
    }

    return 0;
}

RHICommandBuffer vk_start_command_recording(RHIQueue queue) {
    assert(queue);

    auto vulkan_queue = (VulkanQueue *) queue;

    VkCommandBufferAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = vulkan_queue->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer cb = nullptr;
    if (vkAllocateCommandBuffers(vulkan_queue->device->device, &alloc_info, &cb) != VK_SUCCESS) {
        assert(false);
    }

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    if (vkBeginCommandBuffer(cb, &begin_info) != VK_SUCCESS) {
        vkFreeCommandBuffers(vulkan_queue->device->device, vulkan_queue->command_pool, 1, &cb);

        assert(false);
    }

    auto vkcb = (VulkanCommandBuffer *) malloc(sizeof(VulkanCommandBuffer));
    if (!vkcb) {
        vkFreeCommandBuffers(vulkan_queue->device->device, vulkan_queue->command_pool, 1, &cb);

        assert(false);
    }
    vkcb->command_buffer = cb;
    vkcb->queue = (VulkanQueue *) queue;

    return (u64) vkcb;
}

void vk_submit(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count, RHISemaphore sem, u64 sem_val) {
    assert(queue);
    assert(command_buffers);
    assert(command_buffer_count);
    assert(sem);

    auto vulkan_queue = (VulkanQueue *) queue;
    auto semaphore = (VulkanSemaphore *) sem;

    struct SemaphoreSubmitInfo {
        VkSemaphore semaphore;
        u64 value;
        bool operator==(const SemaphoreSubmitInfo &o) const noexcept {
            return semaphore == o.semaphore && value == o.value;
        }
    };
    
    struct SemaphoreSubmitInfoHash {
        size_t operator()(const SemaphoreSubmitInfo & s) const noexcept {
            // Combine pointer-sized semaphore and 64-bit value into a hash.
            // Use uintptr_t to hash semaphore handle portably.
            const auto h1 = std::hash<std::uintptr_t>()((std::uintptr_t) s.semaphore);
            const auto h2 = std::hash<u64>()(s.value);
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };

    // Insert into intermediate maps to deduplicate semaphore value pairs
    std::unordered_map<SemaphoreSubmitInfo, VkPipelineStageFlags2, SemaphoreSubmitInfoHash> signals_map;
    std::unordered_map<SemaphoreSubmitInfo, VkPipelineStageFlags2, SemaphoreSubmitInfoHash> waits_map;
    std::vector<VkCommandBufferSubmitInfo> cb_infos;
    cb_infos.reserve(command_buffer_count);

    SemaphoreSubmitInfo cpu_sem_info = {
        .semaphore = semaphore->semaphore,
        .value = sem_val,
    };

    signals_map[cpu_sem_info] = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    VulkanTexture *backbuffer = nullptr;

    for (u32 i = 0; i < command_buffer_count; i++) {
        auto cb = (VulkanCommandBuffer*) command_buffers[i];

        assert(cb);
        assert(cb->queue == vulkan_queue);

        cb_infos[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        cb_infos[i].commandBuffer = cb->command_buffer;

        if (cb->backbuffer) {
            if (!backbuffer) {
                backbuffer = cb->backbuffer;
            }

            assert(cb->backbuffer == backbuffer);
        }

        for (const auto &wait : cb->waits) {
            SemaphoreSubmitInfo new_wait = {
                .semaphore = wait.semaphore,
                .value = wait.value
            };
            waits_map[new_wait] = waits_map[new_wait] | wait.stageMask;
        }

        for (const auto &signal : cb->signals) {
            SemaphoreSubmitInfo new_signal = {
                .semaphore = signal.semaphore,
                .value = signal.value
            };
            signals_map[new_signal] = waits_map[new_signal] | signal.stageMask;
        }
    }

    if (!backbuffer->backbuffer_data->acquire_consumed) {
        // TODO: We probably want to track pipeline stage here at some point
        SemaphoreSubmitInfo wait = {
            .semaphore = backbuffer->backbuffer_data->acquire_semaphore,
            .value = 0
        };
        SemaphoreSubmitInfo signal = {
            .semaphore = backbuffer->backbuffer_data->acquire_semaphore,
            .value = backbuffer->backbuffer_data->ready_value
        };

        waits_map[wait] = waits_map[wait] | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        signals_map[signal] = signals_map[signal] | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else {
        SemaphoreSubmitInfo wait = {
            .semaphore = backbuffer->backbuffer_data->acquire_semaphore,
            .value = backbuffer->backbuffer_data->ready_value
        };
        backbuffer->backbuffer_data->present_value++;
        SemaphoreSubmitInfo signal = {
            .semaphore = backbuffer->backbuffer_data->acquire_semaphore,
            .value = backbuffer->backbuffer_data->present_value
        };

        waits_map[wait] = waits_map[wait] | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        signals_map[signal] = signals_map[signal] | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    }

    // Do the deduplication
    std::vector<VkSemaphoreSubmitInfo> waits;
    waits.reserve(waits_map.size());
    std::vector<VkSemaphoreSubmitInfo> signals;
    signals.reserve(signals_map.size());

    for (const auto &wait : waits_map) {
        VkSemaphoreSubmitInfo new_wait = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = wait.first.semaphore,
            .value = wait.first.value,
            .stageMask = wait.second
        };
        waits.push_back(new_wait);
    }

    for (const auto &signal : signals_map) {
        VkSemaphoreSubmitInfo new_signal = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = signal.first.semaphore,
            .value = signal.first.value,
            .stageMask = signal.second
        };
        signals.push_back(new_signal);
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
        assert(false);
    }
    
    // TODO: Is it worth collecting command buffers into an array and freeing all of them?
    for (u32 i = 0; i < command_buffer_count; i++) {
        auto cb = (VulkanCommandBuffer *) command_buffers[i];

        assert(cb);
        assert(cb->queue == vulkan_queue);

        vkFreeCommandBuffers(vulkan_queue->device->device, vulkan_queue->command_pool, 1, &cb->command_buffer);
    }
}

void vk_abort(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count) {
    assert(queue);
    assert(command_buffers);
    assert(command_buffer_count);

    auto vulkan_queue = (VulkanQueue *) queue;

    // TODO: Is it worth collecting command buffers into an array and freeing all of them?
    for (u32 i = 0; i < command_buffer_count; i++) {
        auto cb = (VulkanCommandBuffer *) command_buffers[i];

        assert(cb);
        assert(cb->queue == vulkan_queue);
 
        vkFreeCommandBuffers(vulkan_queue->device->device, vulkan_queue->command_pool, 1, &cb->command_buffer);
    }
}

// Swapchain
RHISwapchain vk_create_swapchain(RHIDevice device, RHISwapchainDesc *desc);

void vk_destroy_swapchain(RHIDevice device, RHISwapchain swapchain) {
    assert(device);
    assert(swapchain);

    auto vulkan_device = (VulkanDevice *) device;
    auto vulkan_swapchain = (VulkanSwapchain *) swapchain;

    for (u32 i = 0; i < vulkan_swapchain->image_count; i++) {
        vkDestroyImageView(vulkan_device->device, vulkan_swapchain->textures[i].image_view, nullptr);
        vkDestroySemaphore(vulkan_device->device, vulkan_swapchain->textures[i].backbuffer_data->timeline, nullptr);
        vkDestroySemaphore(vulkan_device->device, vulkan_swapchain->acquire_semaphores[i], nullptr);
        vkDestroySemaphore(vulkan_device->device, vulkan_swapchain->present_semaphores[i], nullptr);

        free(vulkan_swapchain->textures[i].backbuffer_data);
    }

    vkDestroyFence(vulkan_device->device, vulkan_swapchain->fence, nullptr);

    free(vulkan_swapchain->textures);
    free(vulkan_swapchain);
}

// TODO: remove presented flag, move image index to backbuffer data, and add RHITexture as present parameter
RHITexture vk_next_backbuffer(RHISwapchain swapchain) {
    assert(swapchain);

    auto vulkan_swapchain = (VulkanSwapchain *) swapchain;

    VkSemaphore acquire_semaphore = vulkan_swapchain->acquire_semaphores[vulkan_swapchain->semaphore_index];

    u32 index = 0;
    if (vkAcquireNextImageKHR(vulkan_swapchain->device,
        vulkan_swapchain->swapchain,
        UINT64_MAX,
        acquire_semaphore,
        nullptr,
        &index) != VK_SUCCESS)
    {
        assert(false);
    }
    if (vkWaitForFences(vulkan_swapchain->device, 1, &vulkan_swapchain->fence, true, UINT64_MAX) != VK_SUCCESS) {
        assert(false);
    }

    // Assign semaphores from swapchain ringbuffer
    VulkanTexture *texture = &vulkan_swapchain->textures[index];
    texture->backbuffer_data->present_semaphore = vulkan_swapchain->present_semaphores[vulkan_swapchain->semaphore_index];
    texture->backbuffer_data->acquire_semaphore = acquire_semaphore;
    texture->backbuffer_data->acquire_consumed = false;
    texture->backbuffer_data->present_value++;
    texture->backbuffer_data->ready_value = texture->backbuffer_data->present_value;

    vulkan_swapchain->semaphore_index = (vulkan_swapchain->semaphore_index + 1) % vulkan_swapchain->image_count;

    return (RHITexture) texture;
}

// The shit i have to do to implicitly sync present is dumb

void vk_present(RHISwapchain swapchain, RHITexture texture) {
    assert(swapchain);
    assert(texture);

    auto vulkan_swapchain = (VulkanSwapchain *) swapchain;
    auto vulkan_texture = (VulkanTexture *) texture;

    assert(vulkan_texture->backbuffer_data);
    assert(vulkan_texture->backbuffer_data->acquire_semaphore);
    assert(vulkan_texture->backbuffer_data->present_semaphore);

    // Convert timeline semaphore to binary semaphore for present
    if (vulkan_texture->backbuffer_data->acquire_consumed) {
        VkSemaphoreSubmitInfo wait = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = vulkan_texture->backbuffer_data->timeline,
            .value = vulkan_texture->backbuffer_data->present_value
        };

        VkSemaphoreSubmitInfo signal = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = vulkan_texture->backbuffer_data->present_semaphore,
        };

        VkSubmitInfo2 submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &wait,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &signal,
        };

        vkQueueSubmit2(vulkan_swapchain->queue, 1, &submit_info, nullptr);
    }

    // Present
    VkResult result = VK_SUCCESS;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .swapchainCount = 1,
        .pSwapchains = &vulkan_swapchain->swapchain,
        .pImageIndices = &vulkan_texture->backbuffer_data->image_index,
        .pResults = &result,
    };

    if (vulkan_texture->backbuffer_data->acquire_consumed) {
        present_info.pWaitSemaphores = &vulkan_texture->backbuffer_data->present_semaphore;
    } else {
        present_info.pWaitSemaphores = &vulkan_texture->backbuffer_data->acquire_semaphore;
    }

    vkQueuePresentKHR(vulkan_swapchain->queue, &present_info);

    assert(result == VK_SUCCESS);

    vulkan_texture->backbuffer_data->acquire_semaphore = nullptr;
    vulkan_texture->backbuffer_data->present_semaphore = nullptr;
    vulkan_texture->backbuffer_data->acquire_consumed = false;
}

// Semaphores
RHISemaphore vk_create_semaphore(RHIDevice device, u64 init_value) {
    assert(device);

    auto vulkan_device = (VulkanDevice *) device;

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
        assert(false);
    }

    auto semaphore = (VulkanSemaphore *) malloc(sizeof(VulkanSemaphore));

    if (!semaphore) {
        vkDestroySemaphore(vulkan_device->device, sem, nullptr);

        assert(false);
    }

    semaphore->semaphore = sem;

    return (u64) semaphore;
}

void vk_wait_semaphore(RHIDevice device, RHISemaphore sem, u64 value) {
    assert(device);
    assert(sem);

    auto vulkan_device = (VulkanDevice *) device;
    auto semaphore = (VulkanSemaphore *) sem;

    VkSemaphoreWaitInfo info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &semaphore->semaphore,
        .pValues = &value
    };

    if (vkWaitSemaphores(vulkan_device->device, &info, UINT64_MAX) != VK_SUCCESS) {
        assert(false);
    }
}

void vk_destroy_semaphore(RHIDevice device, RHISemaphore sem) {
    assert(device);
    assert(sem);

    auto vulkan_device = (VulkanDevice *) device;
    auto semaphore = (VulkanSemaphore *) sem;

    vkDestroySemaphore(vulkan_device->device, semaphore->semaphore, nullptr);
    free(semaphore);
}

// Commands
void vk_mem_copy(RHICommandBuffer cb, void *dst_gpu, void *src_gpu, u64 size) {
    assert(cb);
    assert(dst_gpu);
    assert(src_gpu);
    assert(size);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, dst_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, src_gpu));

    VulkanBufferOffset dst = get_buffer_offset_gpu(command_buffer->queue->device, dst_gpu);
    VulkanBufferOffset src = get_buffer_offset_gpu(command_buffer->queue->device, src_gpu);

    VkBufferCopy copy = {
        .srcOffset = src.offset,
        .dstOffset = dst.offset,
        .size = size,
    };
    vkCmdCopyBuffer(command_buffer->command_buffer, src.buffer, dst.buffer, 1, &copy);
}

void vk_copy_to_texture(RHICommandBuffer cb, RHITexture texture, void *src_gpu) {
    assert(cb);
    assert(texture);
    assert(src_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto tex = (VulkanTexture *) texture;

    assert(find_allocation_gpu(command_buffer->queue->device, src_gpu));

    VulkanBufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, src_gpu);

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
    vkCmdCopyBufferToImage(command_buffer->command_buffer, offset.buffer, tex->image, VK_IMAGE_LAYOUT_GENERAL, 1, &copy);
}

void vk_copy_from_texture(RHICommandBuffer cb, void *dst_gpu, RHITexture texture) {
    assert(cb);
    assert(dst_gpu);
    assert(texture);

    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto tex = (VulkanTexture *) texture;

    assert(find_allocation_gpu(command_buffer->queue->device, dst_gpu));

    VulkanBufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, dst_gpu);

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
    vkCmdCopyImageToBuffer(command_buffer->command_buffer, tex->image, VK_IMAGE_LAYOUT_GENERAL, offset.buffer, 1, &copy);
}

void vk_set_active_texture_heap_ptr(RHICommandBuffer cb, void *ptr_gpu, u64 size) {
    assert(cb);
    assert(ptr_gpu);
    assert(size);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, ptr_gpu));

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
    assert(cb);

    auto command_buffer = (VulkanCommandBuffer *) cb;

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
    assert(cb);
    assert(sem);

    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto semaphore = (VulkanSemaphore *) sem;

    command_buffer->signals.push_back({ 
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore->semaphore,
        .value = value,
        .stageMask = vk_get_pipeline_stage(before),
    });
}

void vk_wait_before(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value) {
    assert(cb);
    assert(sem);

    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto semaphore = (VulkanSemaphore *) sem;

    command_buffer->waits.push_back({
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore->semaphore,
        .value = value,
        .stageMask = vk_get_pipeline_stage(after),
    });
}

void vk_set_pipeline(RHICommandBuffer cb, RHIPipeline pipeline) {
    assert(cb);
    assert(pipeline);

    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto p = (VulkanPipeline *) pipeline;

    if (command_buffer == nullptr || p == nullptr) {
        return;
    }

    vkCmdBindPipeline(command_buffer->command_buffer, p->bind_point, p->pipeline);
}

void vk_set_depth_stencil_state(RHICommandBuffer cb, RHIDepthStencilState state) {
    assert(cb);
    assert(state);

    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto depth_stencil = (VulkanDepthStencilState *) state;

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
    assert(cb);
    assert(state);

    auto command_buffer = (VulkanCommandBuffer *) cb;
    auto blend = (VulkanBlendState *) state;

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
    assert(cb);
    assert(data_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, data_gpu));

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
    assert(cb);
    assert(data_gpu);
    assert(grid_dimensions_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, grid_dimensions_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, grid_dimensions_gpu));

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
    assert(cb);
    assert(desc);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    VkRect2D render_area = {};
    render_area.extent.width = UINT32_MAX;
    render_area.extent.height = UINT32_MAX;

    std::vector<VkRenderingAttachmentInfo> color_attachments;
    color_attachments.reserve(desc->color_attachment_count);
    for (u32 i = 0; i < desc->color_attachment_count; i++) {
        auto texture = (VulkanTexture *) desc->color_attachments[i].texture;

        assert(texture);

        // track swapchain backbuffer target
        if (texture->backbuffer_data) {
            assert(!command_buffer->backbuffer || command_buffer->backbuffer == texture);
            command_buffer->backbuffer = texture;
        }

        VkRenderingAttachmentInfo attachment = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = texture->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE
        };

        color_attachments.push_back(attachment);

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
    assert(cb);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    vkCmdEndRendering(command_buffer->command_buffer);
}

void vk_draw_indexed_instanced(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count) {
    assert(cb);
    assert(vertex_data_gpu);
    assert(pixel_data_gpu);
    assert(indices_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, vertex_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, indices_gpu));

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
    assert(cb);
    assert(vertex_data_gpu);
    assert(pixel_data_gpu);
    assert(indices_gpu);
    assert(args_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, vertex_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, indices_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, args_gpu));

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
    assert(cb);
    assert(vertex_data_gpu);
    assert(pixel_data_gpu);
    assert(args_gpu);
    assert(draw_count_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, vertex_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, draw_count_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, args_gpu));

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
    assert(cb);
    assert(meshlet_data_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, meshlet_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu));

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
    assert(cb);
    assert(meshlet_data_gpu);
    assert(pixel_data_gpu);
    assert(dim_gpu);

    auto command_buffer = (VulkanCommandBuffer *) cb;

    assert(find_allocation_gpu(command_buffer->queue->device, meshlet_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, pixel_data_gpu));
    assert(find_allocation_gpu(command_buffer->queue->device, dim_gpu));

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


bool vulkan_init(RHI *rhi) {
    bool extensions_supported = false;
    u32 layer_count = 0;

    if (vkEnumerateInstanceLayerProperties(&layer_count, nullptr) != VK_SUCCESS) {
        assert(false);
    }
    auto layers = (VkLayerProperties *) malloc(sizeof(VkLayerProperties) * layer_count);
    if (!layers) {
        free(layers);

        assert(false);
    }
    if (vkEnumerateInstanceLayerProperties(&layer_count, layers) != VK_SUCCESS) {
        free(layers);

        assert(false);
    }

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

    free(layers);

    assert(extensions_supported);

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
        assert(false);
    }

    if (create_debug_messenger(vulkan.instance, &debug_info, nullptr, &vulkan.debug_messenger) != VK_SUCCESS) {
        assert(false);
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

        //Swapchain
        vk_create_swapchain,
        vk_destroy_swapchain,
        vk_next_backbuffer,
        vk_present,

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
        vk_get_queue,
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