#include "VulkanRHI.h"

// TODO: Remove dependency
#include "Log.h"

#include "VulkanPlatform.h"
#include "VulkanLoader.h"

#include "RHIUtils.h"

#ifdef SK_DEBUG
static const bool enable_validation = true;
const u32 validation_layer_count = 1;
const char *validation_layers[validation_layer_count] = {
    "VK_LAYER_KHRONOS_validation"
};
const u32 device_extension_count = 4;
const char *device_extensions[device_extension_count] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
    VK_EXT_MESH_SHADER_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME,
};

#define ASSERT_RESOURCE_GPU(device, ptr) assert((ptr) == NULL || find_allocation_gpu((device), (ptr)))
#define ASSERT_RESOURCE_CPU(device, ptr) assert((ptr) == NULL || find_allocation_cpu((device), (ptr)))
#define ASSERT_RESOURCE_GPU_STRICT(device, ptr) assert((ptr) && find_allocation_gpu((device), (ptr)))
#define ASSERT_RESOURCE_CPU_STRICT(device, ptr) assert((ptr) && find_allocation_cpu((device), (ptr)))

// Check if a resouce is CPU or GPU allocated, but allow null pointers
#define ASSERT_RESOURCE_IS(device, ptr, Type) ASSERT_RESOURCE_##Type##(device, ptr)
// Check if a resource is CPU or GPU allocated, and assert if it's not (including null pointers)
#define ASSERT_RESOURCE_IS_STRICT(device, ptr, Type) ASSERT_RESOURCE_##Type##_STRICT(device, ptr)
#endif

#define palloc(x) vk.alloc->alloc(x, vk.alloc->user_data)
#define pfree(x)  vk.alloc->free((void *)(x), vk.alloc->user_data)

#define talloc(x) vk.temp_alloc->alloc(x, vk.temp_alloc->user_data)
#define treset() vk.temp_alloc->reset(vk.temp_alloc->user_data)

struct Queue;
struct Texture;

struct TempAllocatorData {
    u8 *base;
    u8 *curr;
    u64 capacity;
};

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

    RHIAllocator *alloc;
    RHIAllocator *temp_alloc;
};

struct Swapchain {
    VkDevice device;

    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkQueue queue;
    u32 queue_family;

    u32 image_count;
    Texture *textures;
    VkExtent2D extent;

    u32 semaphore_index;
    VkSemaphore *acquire_semaphores;
    VkSemaphore *present_semaphores;
    VkFence *fences;

    // Used for backbuffer layout transitions
    VkCommandPool command_pool;
    VkCommandBuffer *command_buffers;
};

struct QueueFamilyAssignment {
    u32 family;
    u32 index;
    u32 capabilities;
};

struct QueueFamilyInfo {
    u32 total_count;
    u32 used_count;

    VkQueueFlags flags;
    bool supports_present;
};

struct SwapchainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    rhi::Array<VkSurfaceFormatKHR> formats;
    rhi::Array<VkPresentModeKHR> present_modes;
};

struct Device {
    VkDevice device;
    VkPhysicalDevice gpu;
    VkCommandPool command_pool;

    rhi::DynamicArray<AllocBlock> cpu_allocations;
    rhi::DynamicArray<AllocBlock> gpu_allocations;

    rhi::Array<Queue> queues;
};

struct Semaphore {
    VkSemaphore semaphore;
};

// TODO: Could this use a VulkanCommandBufer instead of VkCommandBuffer to reduce allocs and frees?
struct SubmissionBatch {
    u64 wait_value;
    rhi::Array<VkCommandBuffer> command_buffers;
};

struct Queue {
    VkQueue queue;

    u32 family;
    u32 index;
    u32 capabilities;

    Device *device;

    VkCommandPool command_pool;

    VkSemaphore batch_semaphore;
    u64 next_semaphore_value;
    
    rhi::DynamicArray<SubmissionBatch> batches;

    rhi::DynamicArray<VkCommandBuffer> free_command_buffers;
};

struct BackbufferData {
    bool valid;

    VkSemaphore timeline;

    u64 ready_value;

    u32 image_index;

    // Set by the swapchain when the backbuffer is acquired, used by the queue to know which semaphores to wait/signal on
    VkSemaphore present_semaphore;
    VkFence present_fence;

    VkCommandBuffer command_buffer; // used for layout transitions
};

struct Texture {
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

    VkImageLayout layout;
    u32 queue_index;

    BackbufferData *backbuffer_data; // only populated for swapchain images
};

struct CommandBuffer {
    VkCommandBuffer command_buffer;
    Queue *queue;
    rhi::DynamicArray<VkSemaphoreSubmitInfo> waits;
    rhi::DynamicArray<VkSemaphoreSubmitInfo> signals;

    Texture *backbuffer; // swapchain backbuffer if one is used
};

struct Pipeline {
    VkPipeline pipeline;
    VkPipelineBindPoint bind_point;
};

struct RasterPushConstants {
    void *vert_data;
    void *frag_data;
};

struct MeshPushConstants {
    void *mesh_data;
    void *frag_data;
};

struct ComputePushConstants {
    void *data;
};

struct BufferOffset {
    VkBuffer buffer;
    VkDeviceSize offset;
};

struct BlendState {
    VkBlendFactor src_color_factor;
    VkBlendFactor dst_color_factor;
    VkBlendOp color_op;
    VkBlendFactor src_alpha_factor;
    VkBlendFactor dst_alpha_factor;
    VkBlendOp alpha_op;
    VkColorComponentFlags color_write_mask;
};

struct Stencil {
    u32 write_mask;
    VkStencilOp fail_op;
    VkStencilOp pass_op;
    VkStencilOp depth_fail_op;
    VkCompareOp compare_op;
    u32 reference;
    u32 compare_mask;
};

struct DepthStencilState {
    bool depth_write_enabled;
    VkCompareOp depth_compare_op;
    float depth_bias_constant;
    float depth_bias_clamp;
    float depth_bias_slope;
    Stencil front;
    Stencil back;
};

static Vulkan vk;

static s64 find_present_queue_family(Device *device, VkSurfaceKHR surface) {
    for (u32 i = 0; i < device->queues.count; i++) {
        VkBool32 supports_present = false;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(device->gpu, device->queues.data[i].family, surface, &supports_present) != VK_SUCCESS) {
            assert(false);
        }

        if (supports_present) {
            return device->queues.data[i].family;
        }
    }

    return -1;
}

// Uses vk.temp_alloc but does not reset it
static SwapchainSupport get_swapchain_support(VkPhysicalDevice gpu, VkSurfaceKHR surface) {
    SwapchainSupport support = {};

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &support.capabilities) != VK_SUCCESS) {
        assert(false);
    }

    u32 format_count = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, nullptr) != VK_SUCCESS) {
        assert(false);
    }

    support.formats = rhi::array<VkSurfaceFormatKHR>(format_count, vk.temp_alloc);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, support.formats.data) != VK_SUCCESS) {
        assert(false);
    }

    u32 present_mode_count = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, nullptr) != VK_SUCCESS) {
        assert(false);
    }

    support.present_modes = rhi::array<VkPresentModeKHR>(present_mode_count, vk.temp_alloc);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, support.present_modes.data) != VK_SUCCESS) {
        assert(false);
    }

    return support;
}

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

static AllocBlock *find_allocation_gpu(Device *device, void *gpu) {
    assert(device);
    assert(gpu);

    u64 low = 0;
    u64 high = device->gpu_allocations.count - 1;

    while (low <= high) {
        u64 mid = low + (high - low) / 2;

        auto block = &device->gpu_allocations.data[mid];
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

static AllocBlock *find_allocation_cpu(Device *device, void *cpu) {
    assert(device);
    assert(cpu);

    u64 low = 0;
    u64 high = device->cpu_allocations.count - 1;

    while (low <= high) {
        u64 mid = low + (high - low) / 2;

        auto block = &device->cpu_allocations.data[mid];
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

static BufferOffset get_buffer_offset_gpu(Device *device, void *gpu) {
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

    for (u32 i = 0; i < mem_props.memoryTypeCount; i++) {
        if (((filter & (1 << i)) != 0) && ((mem_props.memoryTypes[i].propertyFlags & flags) == flags)) {
            *memory_type_index = i;
            return true;
        }
    }

    assert(false);

    return false;
}

static u64 find_insert_pos_cpu(Device *device, void *cpu) {
    assert(device);
    assert(cpu);

    u64 low = 0;
    u64 high = device->cpu_allocations.count;

    while (low < high) {
        u64 mid = (low + high) / 2;

        if (device->cpu_allocations.data[mid].cpu < cpu) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low;
}

static u64 find_insert_pos_gpu(Device *device, void *gpu) {
    assert(device);
    assert(gpu);

    u64 low = 0;
    u64 high = device->gpu_allocations.count;

    while (low < high) {
        u64 mid = (low + high) / 2;

        if (device->gpu_allocations.data[mid].gpu < gpu) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low;
}

static void insert_allocation_cpu(Device *device, AllocBlock in) {
    assert(device);

    auto pos = find_insert_pos_cpu(device, in.cpu);
    rhi::dyn_array_insert(&device->cpu_allocations, pos, in, vk.alloc);
}

static void insert_allocation_gpu(Device *device, AllocBlock in) {
    assert(device);

    auto pos = find_insert_pos_gpu(device, in.gpu);
    rhi::dyn_array_insert(&device->gpu_allocations, pos, in, vk.alloc);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT *callback_data, void *user_data) {
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

static bool queue_family_supports(QueueFamilyInfo *family, u32 capabilities) {
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

    if (bytes == 0) {
        return nullptr;
    }

    auto vulkan_device = (Device *) device;

    AllocBlock alloc_block = {};
    alloc_block.size = bytes;
    alloc_block.cpu = nullptr;

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bytes,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
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

    if (!ptr) {
        return;
    }

    auto vulkan_device = (Device *) device;

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

    if (!ptr) {
        return nullptr;
    }

    auto vulkan_device = (Device *) device;

    ASSERT_RESOURCE_IS(vulkan_device, ptr, CPU);

    AllocBlock *block = find_allocation_cpu(vulkan_device, ptr);

    u64 diff = (u64) ptr - (u64) block->cpu;

    return (void *) ((u64) block->gpu + (u64) diff);
}

// Textures
RHITextureSizeAlign vk_texture_size_align(RHIDevice device, RHITextureDesc *desc) {
    assert(false);
    return {};
}

RHITexture vk_create_texture(RHIDevice device, RHITextureDesc *desc, void *ptr_gpu) {
    assert(false);
    return nullptr;
}

void vk_destroy_texture(RHIDevice device, RHITexture texture) {
    assert(false);
}

RHITextureDescriptor vk_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc) {
    assert(device);
    assert(texture);
    assert(desc);

    auto vulkan_device = (Device *) device;
    auto vulkan_texture = (Texture *) texture;

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

    auto vulkan_device = (Device *) device;
    auto vulkan_texture = (Texture *) texture;

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

    auto vulkan_device = (Device *) device;

    VkShaderModuleCreateInfo shader_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = ir_size,
        .pCode = (u32 *) compute_ir,
    };

    VkShaderModule shader_module = nullptr;

    if (vkCreateShaderModule(vulkan_device->device, &shader_info, nullptr, &shader_module) != VK_SUCCESS) {
        assert(false);
    }

    defer {
        vkDestroyShaderModule(vulkan_device->device, shader_module, nullptr);
    };

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
        assert(false);
    }

    Pipeline *pipeline = (Pipeline *) palloc(sizeof(Pipeline));

    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
    return pipeline;
}

RHIPipeline vk_create_graphics_pipeline(RHIDevice device, u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    assert(device);
    assert(vertex_ir);
    assert(vertex_ir_size);
    assert(pixel_ir);
    assert(pixel_ir_size);
    assert(desc);

    auto vulkan_device = (Device *) device;

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

    defer {
        vkDestroyShaderModule(vulkan_device->device, vertex_shader_module, nullptr);
    };

    if (vkCreateShaderModule(vulkan_device->device, &fragment_shader_info, nullptr, &fragment_shader_module) != VK_SUCCESS) {
        assert(false);
    }

    defer {
        vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);
    };

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

    const u32 dynamic_state_base_count = 11;
    const u32 dynamic_state_blend_count = 4;
    u32 dynamic_state_count = dynamic_state_base_count;
    VkDynamicState dynamic_states[dynamic_state_base_count + dynamic_state_blend_count] = {
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

        VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
        VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
        VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT,
        VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT
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
        dynamic_state_count += dynamic_state_blend_count;
    }

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = dynamic_state_count,
        .pDynamicStates = dynamic_states,
    };
    info.pDynamicState = &dynamic_state;

    VkPipeline vulkan_pipeline = nullptr;
    if (vkCreateGraphicsPipelines(vulkan_device->device, nullptr, 1, &info, nullptr, &vulkan_pipeline) != VK_SUCCESS) {
        assert(false);
    }

    vkDestroyShaderModule(vulkan_device->device, vertex_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

    Pipeline *pipeline = (Pipeline *) palloc(sizeof(Pipeline));

    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return pipeline;
}

RHIPipeline vk_create_graphics_meshlet_pipeline(RHIDevice device, u8 *meshlet_ir, u32 meshlet_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc) {
    assert(device);
    assert(meshlet_ir);
    assert(meshlet_ir_size);
    assert(pixel_ir);
    assert(pixel_ir_size);
    assert(desc);

    auto vulkan_device = (Device *) device;

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

    defer {
        vkDestroyShaderModule(vulkan_device->device, mesh_shader_module, nullptr);
    };

    if (vkCreateShaderModule(vulkan_device->device, &fragment_shader_info, nullptr, &fragment_shader_module) != VK_SUCCESS) {
        assert(false);
    }

    defer {
        vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);
    };

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

    const u32 dynamic_state_base_count = 11;
    const u32 dynamic_state_blend_count = 4;
    u32 dynamic_state_count = dynamic_state_base_count;
    VkDynamicState dynamic_states[dynamic_state_base_count + dynamic_state_blend_count] = {
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

        VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
        VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
        VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT,
        VK_DYNAMIC_STATE_COLOR_WRITE_ENABLE_EXT
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
        dynamic_state_count += dynamic_state_blend_count;
    }

    VkPipelineDynamicStateCreateInfo dynamic_state = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = dynamic_state_count,
        .pDynamicStates = dynamic_states,
    };
    info.pDynamicState = &dynamic_state;

    VkPipeline vulkan_pipeline = nullptr;
    if (vkCreateGraphicsPipelines(vulkan_device->device, nullptr, 1, &info, nullptr, &vulkan_pipeline) != VK_SUCCESS) {
        assert(false);
    }

    vkDestroyShaderModule(vulkan_device->device, mesh_shader_module, nullptr);
    vkDestroyShaderModule(vulkan_device->device, fragment_shader_module, nullptr);

    Pipeline *pipeline = (Pipeline *) palloc(sizeof(Pipeline));

    pipeline->pipeline = vulkan_pipeline;
    pipeline->bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return pipeline;
}

void vk_destroy_pipeline(RHIDevice device, RHIPipeline pipeline) {
    assert(device);
    assert(pipeline);

    Device *vulkan_device = (Device *) device;
    Pipeline *vulkan_pipeline = (Pipeline *) pipeline;

    vkDestroyPipeline(vulkan_device->device, vulkan_pipeline->pipeline, nullptr);

    pfree(vulkan_pipeline);
}

// State objects
RHIDepthStencilState vk_create_depth_stencil_state(RHIDevice device, RHIDepthStencilDesc *desc) {
    assert(device);
    assert(desc);

    DepthStencilState *state = (DepthStencilState *) palloc(sizeof(DepthStencilState));

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

    return state;
}

RHIBlendState vk_create_blend_state(RHIDevice device, RHIBlendDesc *desc) {
    assert(device);
    assert(desc);

    BlendState *state = (BlendState *) palloc(sizeof(BlendState));

    state->src_color_factor = vk_get_blend_factor(desc->src_color_factor);
    state->dst_color_factor = vk_get_blend_factor(desc->src_color_factor);
    state->color_op = vk_get_blend_op(desc->color_op);
    state->src_alpha_factor = vk_get_blend_factor(desc->src_alpha_factor);
    state->dst_alpha_factor = vk_get_blend_factor(desc->dst_alpha_factor);
    state->alpha_op = vk_get_blend_op(desc->alpha_op);
    state->color_write_mask = desc->color_write_mask;

    return state;
}

void vk_free_depth_stencil_state(RHIDevice device, RHIDepthStencilState state) {
    assert(device);
    assert(state);

    DepthStencilState *depth_stencil = (DepthStencilState *) state;

    pfree(depth_stencil);
}

void vk_free_blend_state(RHIDevice device, RHIBlendState state) {
    assert(device);
    assert(state);

    BlendState *blend = (BlendState *) state;

    pfree(blend);
}

// Device
// TODO: Make sure device has present queue built in for swapchain support
RHIDevice vk_create_device(RHIDeviceDesc *desc) {
    defer {
        treset();
    };

    u32 gpu_count = 0;
    if (vkEnumeratePhysicalDevices(vk.instance, &gpu_count, nullptr) != VK_SUCCESS) {
        assert(false);
    }
    
    rhi::Array<VkPhysicalDevice> gpus = rhi::array<VkPhysicalDevice>(gpu_count, vk.temp_alloc);

    if (vkEnumeratePhysicalDevices(vk.instance, &gpu_count, gpus.data) != VK_SUCCESS) {
        assert(false);
    }

    VkPhysicalDevice gpu = nullptr;

    rhi::Array<QueueFamilyAssignment> selected_queue_assignments;
    u32 selected_queue_assignment_count = 0;

    rhi::Array<QueueFamilyInfo> selected_family_infos;
    u32 selected_family_count = 0;

    for (u32 i = 0; i < gpu_count; i++) {
        u32 extension_count = 0;

        if (vkEnumerateDeviceExtensionProperties(gpus.data[i], nullptr, &extension_count, nullptr) != VK_SUCCESS) {
            assert(false);
        }

        rhi::Array<VkExtensionProperties> extensions = rhi::array<VkExtensionProperties>(extension_count, vk.temp_alloc);

        if (vkEnumerateDeviceExtensionProperties(gpus.data[i], nullptr, &extension_count, extensions.data) != VK_SUCCESS) {
            assert(false);
        }

        bool extensions_supported = true;

        for (u32 k = 0; k < device_extension_count; k++) {
            bool extension_supported = false;
            for (u32 j = 0; j < extension_count; j++) {
                if (strcmp(device_extensions[k], extensions.data[j].extensionName) == 0) {
                    extension_supported = true;
                }
            }

            if (!extension_supported) {
                extensions_supported = false;
                break;
            }
        }

        VkPhysicalDeviceDescriptorHeapFeaturesEXT descriptor_heap_features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT,
        };
        VkPhysicalDeviceMeshShaderFeaturesEXT mesh_features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
            .pNext = &descriptor_heap_features,
        };
        VkPhysicalDeviceVulkan14Features features14 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
            .pNext = &mesh_features,
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

        vkGetPhysicalDeviceFeatures2(gpus.data[i], &features);

        if (!features.features.samplerAnisotropy || !features.features.depthBiasClamp || !features.features.depthClamp || !features.features.drawIndirectFirstInstance || !features.features.fullDrawIndexUint32 || !features12.bufferDeviceAddress || !features12.timelineSemaphore || !features12.drawIndirectCount || !features14.maintenance5 || !mesh_features.meshShader || !mesh_features.taskShader || !descriptor_heap_features.descriptorHeap) {
            continue;
        }

        u32 family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpus.data[i], &family_count, nullptr);

        rhi::Array<VkQueueFamilyProperties> families = rhi::array<VkQueueFamilyProperties>(family_count, vk.temp_alloc);

        vkGetPhysicalDeviceQueueFamilyProperties(gpus.data[i], &family_count, families.data);

        rhi::Array<QueueFamilyInfo> family_infos = rhi::array<QueueFamilyInfo>(family_count, vk.temp_alloc);

        for (u32 j = 0; j < family_count; j++) {
            family_infos.data[j].total_count = families.data[j].queueCount;

            family_infos.data[j].used_count = 0;
            family_infos.data[j].flags = families.data[j].queueFlags;

            family_infos.data[j].supports_present = false;
        }

        // TODO: Remove hardcoded size
        rhi::Array<QueueFamilyAssignment> queue_assignments = rhi::array<QueueFamilyAssignment>(family_count, vk.temp_alloc);

        u32 assignment_count = 0;

        bool queues_supported = true;

        for (u32 j = 0; j < desc->queue_count; j++) {
            RHIQueueRequest *request = &desc->queues[j];

            for (u32 k = 0; k < request->count; k++) {
                bool found = false;

                for (u32 family = 0; family < family_count; family++) {
                    QueueFamilyInfo *info = &family_infos.data[family];

                    if (!queue_family_supports(info, request->capabilities)) {
                        continue;
                    }

                    if (info->used_count >= info->total_count) {
                        continue;
                    }

                    QueueFamilyAssignment assignment = {};
                    assignment.family = family;
                    assignment.index = info->used_count;
                    assignment.capabilities = request->capabilities;

                    queue_assignments.data[assignment_count++] = assignment;

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

        if (extensions_supported && queues_supported) {
            gpu = gpus.data[i];
            selected_queue_assignments = queue_assignments;
            selected_queue_assignment_count = assignment_count;
            selected_family_infos = family_infos;
            selected_family_count = family_count;
            break;
        }
    }

    if (!gpu) {
        assert(false);
    }

    VkPhysicalDeviceDescriptorHeapFeaturesEXT descriptor_heap_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT,
        .descriptorHeap = true,
    };
    VkPhysicalDeviceMeshShaderFeaturesEXT mesh_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT,
        .pNext = &descriptor_heap_features,
        .taskShader = true,
        .meshShader = true,
    };
    VkPhysicalDeviceVulkan14Features features14 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = &mesh_features,
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
    rhi::Array<float> priorities = rhi::array<float>(64, vk.temp_alloc);

    for (u32 i = 0; i < 64; i++) {
        priorities.data[i] = 1.0f;
    }

    rhi::DynamicArray<VkDeviceQueueCreateInfo> queue_infos = rhi::dyn_array<VkDeviceQueueCreateInfo>(0, selected_family_count, vk.temp_alloc);

    for (u32 i = 0; i < selected_family_count; i++) {

        QueueFamilyInfo *family = &selected_family_infos.data[i];

        if (family->used_count == 0) {
            continue;
        }

        VkDeviceQueueCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = i,
            .queueCount = family->used_count,
            .pQueuePriorities = priorities.data
        };

        rhi::dyn_array_push_back(&queue_infos, info, vk.temp_alloc);
    }

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,

        .queueCreateInfoCount = (u32) queue_infos.count,
        .pQueueCreateInfos = queue_infos.data,

        .enabledExtensionCount = device_extension_count,
        .ppEnabledExtensionNames = device_extensions,
    };

    VkDevice vk_device = nullptr;

    if (vkCreateDevice(gpu, &create_info, nullptr, &vk_device) != VK_SUCCESS) {
        assert(false);
    }

    // load device-level extension symbols (required for swapchain / EXT functions used later)
    if (!load_device_functions(vk_device)) {
        vkDestroyDevice(vk_device, nullptr);

        SK_LOG_ERROR("Failed to load device-level Vulkan functions");

        assert(false);
    }

    Device *device = (Device *) palloc(sizeof(Device));

    device->device = vk_device;
    device->gpu = gpu;

    device->queues = rhi::array<Queue>(selected_queue_assignment_count, vk.alloc);

    for (u32 i = 0; i < selected_queue_assignment_count; i++) {
        QueueFamilyAssignment *assignment = &selected_queue_assignments.data[i];

        Queue *queue = &device->queues.data[i];

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
                vkDestroyCommandPool(vk_device, device->queues.data[j].command_pool, nullptr);
            }
            vkDestroyDevice(vk_device, nullptr);

            rhi::array_free(&device->queues, vk.alloc);
            pfree(device);

            assert(false);
        }

        VkSemaphoreTypeCreateInfo timeline_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = 0,
        };
        VkSemaphoreCreateInfo semaphore_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &timeline_info,
        };

        if (vkCreateSemaphore(device->device, &semaphore_info, nullptr, &queue->batch_semaphore) != VK_SUCCESS) {
            for (u32 j = 0; j <= i; j++) {
                vkDestroyCommandPool(vk_device, device->queues.data[j].command_pool, nullptr);
            }
            vkDestroyDevice(vk_device, nullptr);

            rhi::array_free(&device->queues, vk.alloc);
            pfree(device);

            assert(false);
        }

        queue->next_semaphore_value = 1;
    }

    return device;
}

void vk_destroy_device(RHIDevice device) {
    assert(device);

    auto vulkan_device = (Device *) device;

    for (u32 i = 0; i < vulkan_device->queues.count; i++) {
        vkDestroyCommandPool(vulkan_device->device, vulkan_device->queues.data[i].command_pool, nullptr);
        vkDestroySemaphore(vulkan_device->device, vulkan_device->queues.data[i].batch_semaphore, nullptr);
    }

    rhi::array_free(&vulkan_device->queues, vk.alloc);

    // TODO: Add memory leak detection
    rhi::dyn_array_free(&vulkan_device->gpu_allocations, vk.alloc);
    rhi::dyn_array_free(&vulkan_device->cpu_allocations, vk.alloc);

    vkDestroyDevice(vulkan_device->device, nullptr);

    pfree(vulkan_device);
}

void vk_device_wait_idle(RHIDevice device) {
    assert(device);

    auto vulkan_device = (Device *) device;

    vkDeviceWaitIdle(vulkan_device->device);
}

// Queue
RHIQueue vk_get_queue(RHIDevice device, RHIQueueDesc *desc) {
    assert(device);
    assert(desc);

    auto vulkan_device = (Device *) device;

    u32 found = 0;

    for (u32 i = 0; i < vulkan_device->queues.count; i++) {
        if ((vulkan_device->queues.data[i].capabilities & desc->capabilities) != desc->capabilities) {
            continue;
        }

        if (found == desc->index) {
            return &vulkan_device->queues.data[i];
        }

        found++;
    }

    assert(false);

    return nullptr;
}

RHICommandBuffer vk_start_command_recording(RHIQueue queue) {
    assert(queue);

    auto vulkan_queue = (Queue *) queue;

    // Check if we can free any batches before allocating new command buffers
    bool batch_finished = true;
    while (batch_finished && vulkan_queue->batches.count > 0) {
        SubmissionBatch *batch = rhi::dyn_array_front(&vulkan_queue->batches);

        u64 value = 0;
        if (vkGetSemaphoreCounterValue(vulkan_queue->device->device, vulkan_queue->batch_semaphore, &value) != VK_SUCCESS) {
            assert(false);
        }
        
        if (value >= batch->wait_value) {
            for (u32 i = 0; i < batch->command_buffers.count; i++) {
                vkResetCommandBuffer(batch->command_buffers.data[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
                rhi::dyn_array_push_back<VkCommandBuffer>(&vulkan_queue->free_command_buffers, batch->command_buffers.data[i], vk.alloc);
            }
            rhi::array_free(&batch->command_buffers, vk.alloc);
            rhi::dyn_array_pop_front(&vulkan_queue->batches);
        } else {
            batch_finished = false;
        }
    }

    VkCommandBuffer cb = nullptr;

    if (vulkan_queue->free_command_buffers.count > 0) {
        cb = *rhi::dyn_array_front(&vulkan_queue->free_command_buffers);
        rhi::dyn_array_pop_front(&vulkan_queue->free_command_buffers);
    } else {
        VkCommandBufferAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = vulkan_queue->command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        if (vkAllocateCommandBuffers(vulkan_queue->device->device, &alloc_info, &cb) != VK_SUCCESS) {
            assert(false);
        }
    }

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    if (vkBeginCommandBuffer(cb, &begin_info) != VK_SUCCESS) {
        vkFreeCommandBuffers(vulkan_queue->device->device, vulkan_queue->command_pool, 1, &cb);

        assert(false);
    }

    CommandBuffer *vkcb = (CommandBuffer *) palloc(sizeof(CommandBuffer));

    vkcb->command_buffer = cb;
    vkcb->queue = (Queue *) queue;

    return vkcb;
}

void vk_submit(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count, RHISemaphore sem, u64 sem_val) {
    defer {
        treset();
    };

    assert(queue);
    assert(command_buffers);
    assert(command_buffer_count);
    assert(sem);

    auto vulkan_queue = (Queue *) queue;
    auto semaphore = (Semaphore *) sem;

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
    // Duplication is possible because we implicitly track semaphore usages at the command buffer level, 
    //   and the caller should be allowed to call wait/signal on whatever command buffer they want without having to worry 
    //   about the implicit synchronization that we track for them, and deduplicating on our end allows us to simplify the implementation 
    //   for the caller while still ensuring correctness and avoiding redundant waits/signals.
    std::unordered_map<SemaphoreSubmitInfo, VkPipelineStageFlags2, SemaphoreSubmitInfoHash> signals_map;
    std::unordered_map<SemaphoreSubmitInfo, VkPipelineStageFlags2, SemaphoreSubmitInfoHash> waits_map;
    // We do not deduplicate command buffers, 
    //   because the caller is able to manage that themselves by merging command buffers if they choose to, 
    //   and it simplifies the implementation on our end to just submit them as is.
    rhi::Array<VkCommandBufferSubmitInfo> cb_infos = rhi::array<VkCommandBufferSubmitInfo>(command_buffer_count, vk.temp_alloc);

    SemaphoreSubmitInfo cpu_sem_info = {
        .semaphore = semaphore->semaphore,
        .value = sem_val,
    };

    signals_map[cpu_sem_info] = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    Texture *backbuffer = nullptr;

    for (u32 i = 0; i < command_buffer_count; i++) {
        auto cb = (CommandBuffer*) command_buffers[i];

        assert(cb);
        assert(cb->queue == vulkan_queue);

        vkEndCommandBuffer(cb->command_buffer);

        cb_infos.data[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        cb_infos.data[i].commandBuffer = cb->command_buffer;

        if (cb->backbuffer) {
            if (!backbuffer) {
                backbuffer = cb->backbuffer;
            }

            assert(cb->backbuffer == backbuffer);
        }

        for (u32 i = 0; i < cb->waits.count; i++) {
            SemaphoreSubmitInfo new_wait = {
                .semaphore = cb->waits.data[i].semaphore,
                .value = cb->waits.data[i].value
            };
            waits_map[new_wait] = waits_map[new_wait] | cb->waits.data[i].stageMask;
        }

        for (u32 i = 0; i < cb->signals.count; i++) {
            SemaphoreSubmitInfo new_signal = {
                .semaphore = cb->signals.data[i].semaphore,
                .value = cb->signals.data[i].value
            };
            signals_map[new_signal] = waits_map[new_signal] | cb->signals.data[i].stageMask;
        }

        // We no longer need the CommandBuffer object after extracting the synchronization info and command buffer handle, 
        //   so we can free it now to avoid having to track it later when we want to reuse the command buffer handle.
        pfree(cb);
    }

    assert(!backbuffer || backbuffer->backbuffer_data->valid);

    // I Dont think it is possible to duplicate the backbuffer semaphores, but just in case, we will insert them into the maps as well. 
    if (backbuffer) {
        // TODO: We probably want to track pipeline stage here at some point
        SemaphoreSubmitInfo wait = {
            .semaphore = backbuffer->backbuffer_data->timeline,
            .value = 0
        };
        backbuffer->backbuffer_data->ready_value++;
        SemaphoreSubmitInfo signal = {
            .semaphore = backbuffer->backbuffer_data->timeline,
            .value = backbuffer->backbuffer_data->ready_value
        };

        waits_map[wait] = waits_map[wait] | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        signals_map[signal] = signals_map[signal] | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    }

    // Do the deduplication
    u32 wait_count = waits_map.size();
    u32 signal_count = signals_map.size() + 1;

    rhi::Array<VkSemaphoreSubmitInfo> waits = rhi::array<VkSemaphoreSubmitInfo>(wait_count, vk.temp_alloc);
    rhi::Array<VkSemaphoreSubmitInfo> signals = rhi::array<VkSemaphoreSubmitInfo>(signal_count, vk.temp_alloc);

    u32 wait_index = 0;
    for (const auto &wait : waits_map) {
        waits.data[wait_index] = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = wait.first.semaphore,
            .value = wait.first.value,
            .stageMask = wait.second
        };
        wait_index += 1;
    }

    u32 signal_index = 0;
    for (const auto &signal : signals_map) {
        signals.data[signal_index] = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = signal.first.semaphore,
            .value = signal.first.value,
            .stageMask = signal.second
        };
        signal_index += 1;
    }

    SubmissionBatch batch = {
        .wait_value = vulkan_queue->next_semaphore_value,
        .command_buffers = rhi::array<VkCommandBuffer>(command_buffer_count, vk.alloc),
    };

    for (u32 i = 0; i < batch.command_buffers.count; i++) {
        batch.command_buffers.data[i] = cb_infos.data[i].commandBuffer;
    }

    // This definitely can't be duplicated so we add it directly to the list of signals
    signals.data[signal_count - 1] = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = vulkan_queue->batch_semaphore,
        .value = vulkan_queue->next_semaphore_value,
    };

    vulkan_queue->next_semaphore_value += 1;

    VkSubmitInfo2 info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = (u32) waits.count,
        .pWaitSemaphoreInfos = waits.data,
        .commandBufferInfoCount = (u32) cb_infos.count,
        .pCommandBufferInfos = cb_infos.data,
        .signalSemaphoreInfoCount = (u32) signals.count,
        .pSignalSemaphoreInfos = signals.data,
    };
    
    if (vkQueueSubmit2(vulkan_queue->queue, 1, &info, nullptr) != VK_SUCCESS) {
        assert(false);
    }

    rhi::dyn_array_push_back(&vulkan_queue->batches, batch, vk.alloc);
}

// Swapchain
RHISwapchain vk_create_swapchain(RHIDevice device, RHISwapchainDesc *desc) {
    defer {
        treset();
    };

    assert(device);
    assert(desc);

    Device *vulkan_device = (Device *) device;

    Swapchain *swapchain = (Swapchain *) palloc(sizeof(Swapchain));

    swapchain->device = vulkan_device->device;
    if (!platform_create_surface(vk.instance, desc->window, &swapchain->surface)) {
        assert(false);
    }

    SwapchainSupport support = get_swapchain_support(vulkan_device->gpu, swapchain->surface);

    VkSurfaceFormatKHR surface_format = {};
    bool format_found = false;
    for (u32 i = 0; i < support.formats.count; i++) {
        if (support.formats.data[i].format == get_format(desc->format) && support.formats.data[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surface_format = support.formats.data[i];
            format_found = true;
            break;
        }
    }

    if (!format_found) {
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain);
     
        assert(false);
    }

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    bool present_mode_found = true;
    if (!desc->vsync) {
        present_mode_found = false;
        for (u32 i = 0; i < support.present_modes.count; i++) {
            // Mailbox is the best option for no vsync, but if it's not available we can settle for immediate
            if (support.present_modes.data[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                present_mode = support.present_modes.data[i];
                present_mode_found = true;
                break;
            }
            if (support.present_modes.data[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                present_mode = support.present_modes.data[i];
                present_mode_found = true;
            }
        }
    }

    if (present_mode == VK_PRESENT_MODE_FIFO_KHR && !desc->vsync) {
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain);

        assert(false);
    }

    if (!present_mode_found) {
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain);
     
        assert(false);
    }

    if (support.capabilities.currentExtent.width == UINT32_MAX) {
        swapchain->extent.width = desc->width;
        swapchain->extent.height = desc->height;
    } else {
        swapchain->extent.width = support.capabilities.currentExtent.width;
        swapchain->extent.height = support.capabilities.currentExtent.height;
    }

    if (desc->image_count < support.capabilities.minImageCount) {
        swapchain->image_count = support.capabilities.minImageCount;
    } else if (desc->image_count > support.capabilities.maxImageCount) {
        swapchain->image_count = support.capabilities.maxImageCount;
    } else {
        swapchain->image_count = desc->image_count;
    }

    VkSwapchainCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = swapchain->surface,
        .minImageCount = swapchain->image_count,
        .imageFormat = surface_format.format,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent = swapchain->extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .preTransform = support.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = true,
    };

    u32 queue_family_count = vulkan_device->queues.count;
    rhi::Array<u32> queue_family_indices = rhi::array<u32>(queue_family_count, vk.temp_alloc);

    for (u32 i = 0; i < vulkan_device->queues.count; i++) {
        queue_family_indices.data[i] = vulkan_device->queues.data[i].family;
    }

    if (vulkan_device->queues.count > 1) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = queue_family_count;
        create_info.pQueueFamilyIndices = queue_family_indices.data;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    s64 present_queue_family = find_present_queue_family(vulkan_device, swapchain->surface);
    if (present_queue_family < 0) {
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain);

        assert(false);
    }

    swapchain->queue_family = present_queue_family;

    vkGetDeviceQueue(vulkan_device->device, (u32) present_queue_family, 0, &swapchain->queue);

    if (vkCreateSwapchainKHR(vulkan_device->device, &create_info, nullptr, &swapchain->swapchain) != VK_SUCCESS) {
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain);

        assert(false);
    }

    if (vkGetSwapchainImagesKHR(vulkan_device->device, swapchain->swapchain, &swapchain->image_count, nullptr) != VK_SUCCESS) {
        vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain);

        assert(false);
    }

    VkImage *images = (VkImage *) talloc(sizeof(VkImage) * swapchain->image_count);

    if (vkGetSwapchainImagesKHR(vulkan_device->device, swapchain->swapchain, &swapchain->image_count, images) != VK_SUCCESS) {
        vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain);
        pfree(images);

        assert(false);
    }

    swapchain->textures = (Texture *) palloc(sizeof(Texture) * swapchain->image_count);
    swapchain->acquire_semaphores = (VkSemaphore *) palloc(sizeof(VkSemaphore) * swapchain->image_count);
    swapchain->present_semaphores = (VkSemaphore *) palloc(sizeof(VkSemaphore) * swapchain->image_count);
    swapchain->fences = (VkFence *) palloc(sizeof(VkFence) * swapchain->image_count);

    for (u32 i = 0; i < swapchain->image_count; i++) {
        swapchain->textures[i].backbuffer_data = (BackbufferData *) palloc(sizeof(BackbufferData));
        swapchain->textures[i].format = surface_format.format;
        swapchain->textures[i].extent = { swapchain->extent.width, swapchain->extent.height, 0 };
        swapchain->textures[i].image = images[i];
        swapchain->textures[i].aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
        swapchain->textures[i].layers = 1;
        swapchain->textures[i].mip_count = 1;
        swapchain->textures[i].sample_count = 1;
        swapchain->textures[i].usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchain->textures[i].type = VK_IMAGE_TYPE_2D;

        VkSemaphoreTypeCreateInfo timeline_type_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
            .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
            .initialValue = 0,
        };

        swapchain->textures[i].backbuffer_data->ready_value = 0;

        VkSemaphoreCreateInfo timeline_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &timeline_type_info,
        };

        if (vkCreateSemaphore(vulkan_device->device, &timeline_info, nullptr, &swapchain->textures[i].backbuffer_data->timeline) != VK_SUCCESS) {
            for (u32 j = 0; j < i; j++) {
                pfree(swapchain->textures[j].backbuffer_data);
            }
            vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
            vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
            pfree(swapchain->textures[i].backbuffer_data);
            pfree(swapchain->present_semaphores);
            pfree(swapchain->acquire_semaphores);
            pfree(swapchain->textures);
            pfree(swapchain->fences);
            pfree(swapchain);

            assert(false);
        }

        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->textures[i].image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surface_format.format,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        if (vkCreateImageView(vulkan_device->device, &view_info, nullptr, &swapchain->textures[i].image_view) != VK_SUCCESS) {
            for (u32 j = 0; j < i; j++) {
                vkDestroySemaphore(vulkan_device->device, swapchain->textures[j].backbuffer_data->timeline, nullptr);
                vkDestroyImageView(vulkan_device->device, swapchain->textures[j].image_view, nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->present_semaphores[j], nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[j], nullptr);
                vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
                vkDestroyFence(vulkan_device->device, swapchain->fences[j], nullptr);
                pfree(swapchain->textures[j].backbuffer_data);
            }
            vkDestroySemaphore(vulkan_device->device, swapchain->textures[i].backbuffer_data->timeline, nullptr);
            vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
            vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
            pfree(swapchain->textures[i].backbuffer_data);
            pfree(swapchain->present_semaphores);
            pfree(swapchain->acquire_semaphores);
            pfree(swapchain->textures);
            pfree(swapchain->fences);
            pfree(swapchain);

            assert(false);
        }
        
        VkSemaphoreCreateInfo semaphore_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        VkFenceCreateInfo fence_info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };

        if (vkCreateSemaphore(vulkan_device->device, &semaphore_info, nullptr, &swapchain->acquire_semaphores[i]) != VK_SUCCESS) {
            for (u32 j = 0; j <= i; j++) {
                vkDestroySemaphore(vulkan_device->device, swapchain->textures[j].backbuffer_data->timeline, nullptr);
                vkDestroyImageView(vulkan_device->device, swapchain->textures[j].image_view, nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->present_semaphores[j], nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[j], nullptr);
                vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
                vkDestroyFence(vulkan_device->device, swapchain->fences[j], nullptr);
                pfree(swapchain->textures[j].backbuffer_data);
            }
            vkDestroySemaphore(vulkan_device->device, swapchain->textures[i].backbuffer_data->timeline, nullptr);
            vkDestroyImageView(vulkan_device->device, swapchain->textures[i].image_view, nullptr);
            vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
            vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
            pfree(swapchain->textures[i].backbuffer_data);
            pfree(swapchain->present_semaphores);
            pfree(swapchain->acquire_semaphores);
            pfree(swapchain->textures);
            pfree(swapchain->fences);
            pfree(swapchain);

            assert(false);
        }

        if (vkCreateSemaphore(vulkan_device->device, &semaphore_info, nullptr, &swapchain->present_semaphores[i]) != VK_SUCCESS) {
            for (u32 j = 0; j <= i; j++) {
                vkDestroySemaphore(vulkan_device->device, swapchain->textures[j].backbuffer_data->timeline, nullptr);
                vkDestroyImageView(vulkan_device->device, swapchain->textures[j].image_view, nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->present_semaphores[j], nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[j], nullptr);
                vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
                vkDestroyFence(vulkan_device->device, swapchain->fences[j], nullptr);
                pfree(swapchain->textures[j].backbuffer_data);
            }
            vkDestroySemaphore(vulkan_device->device, swapchain->textures[i].backbuffer_data->timeline, nullptr);
            vkDestroyImageView(vulkan_device->device, swapchain->textures[i].image_view, nullptr);
            vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[i], nullptr);
            vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
            vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
            pfree(swapchain->textures[i].backbuffer_data);
            pfree(swapchain->present_semaphores);
            pfree(swapchain->acquire_semaphores);
            pfree(swapchain->textures);
            pfree(swapchain->fences);
            pfree(swapchain);

            assert(false);
        }

        if (vkCreateFence(vulkan_device->device, &fence_info, nullptr, &swapchain->fences[i]) != VK_SUCCESS) {
            for (u32 j = 0; j <= i; j++) {
                vkDestroySemaphore(vulkan_device->device, swapchain->textures[j].backbuffer_data->timeline, nullptr);
                vkDestroyImageView(vulkan_device->device, swapchain->textures[j].image_view, nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->present_semaphores[j], nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[j], nullptr);
                vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
                vkDestroyFence(vulkan_device->device, swapchain->fences[j], nullptr);
                pfree(swapchain->textures[j].backbuffer_data);
            }
            vkDestroySemaphore(vulkan_device->device, swapchain->textures[i].backbuffer_data->timeline, nullptr);
            vkDestroyImageView(vulkan_device->device, swapchain->textures[i].image_view, nullptr);
            vkDestroySemaphore(vulkan_device->device, swapchain->present_semaphores[i], nullptr);
            vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[i], nullptr);
            vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
            vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
            pfree(swapchain->textures[i].backbuffer_data);
            pfree(swapchain->present_semaphores);
            pfree(swapchain->acquire_semaphores);
            pfree(swapchain->textures);
            pfree(swapchain->fences);
            pfree(swapchain);

            assert(false);
        }
    }

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = (u32) swapchain->queue_family,
    };
    if (vkCreateCommandPool(vulkan_device->device, &pool_info, nullptr, &swapchain->command_pool) != VK_SUCCESS) {
        for (u32 i = 0; i < swapchain->image_count; i++) {
            vkDestroySemaphore(vulkan_device->device, swapchain->textures[i].backbuffer_data->timeline, nullptr);
            vkDestroyImageView(vulkan_device->device, swapchain->textures[i].image_view, nullptr);
            vkDestroySemaphore(vulkan_device->device, swapchain->present_semaphores[i], nullptr);
            vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[i], nullptr);
            vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
            vkDestroyFence(vulkan_device->device, swapchain->fences[i], nullptr);
            pfree(swapchain->textures[i].backbuffer_data);
        }
        vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
        vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
        pfree(swapchain->present_semaphores);
        pfree(swapchain->acquire_semaphores);
        pfree(swapchain->textures);
        pfree(swapchain->fences);
        pfree(swapchain);

        assert(false);
    }

    swapchain->command_buffers = (VkCommandBuffer *) palloc(sizeof(VkCommandBuffer) * swapchain->image_count);

    for (u32 i = 0; i < swapchain->image_count; i++) {
        VkCommandBufferAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = swapchain->command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        if (vkAllocateCommandBuffers(vulkan_device->device, &alloc_info, &swapchain->command_buffers[i]) != VK_SUCCESS) {
            for (u32 j = 0; j <= i; j++) {
                vkFreeCommandBuffers(vulkan_device->device, swapchain->command_pool, 1, &swapchain->command_buffers[j]);
                vkDestroySemaphore(vulkan_device->device, swapchain->textures[i].backbuffer_data->timeline, nullptr);
                vkDestroyImageView(vulkan_device->device, swapchain->textures[i].image_view, nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->present_semaphores[i], nullptr);
                vkDestroySemaphore(vulkan_device->device, swapchain->acquire_semaphores[i], nullptr);
                vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
                vkDestroyFence(vulkan_device->device, swapchain->fences[i], nullptr);
                pfree(swapchain->textures[i].backbuffer_data);
            }
            vkDestroySwapchainKHR(vulkan_device->device, swapchain->swapchain, nullptr);
            vkDestroySurfaceKHR(vk.instance, swapchain->surface, nullptr);
            pfree(swapchain->present_semaphores);
            pfree(swapchain->acquire_semaphores);
            pfree(swapchain->command_buffers);
            pfree(swapchain->textures);
            pfree(swapchain->fences);
            pfree(swapchain);

            assert(false);
        }
    }

    return swapchain;
}

void vk_destroy_swapchain(RHIDevice device, RHISwapchain swapchain) {
    assert(device);
    assert(swapchain);

    auto vulkan_device = (Device *) device;
    auto vulkan_swapchain = (Swapchain *) swapchain;

    assert(vulkan_swapchain->device == vulkan_device->device);
    assert(vulkan_swapchain->textures);

    for (u32 i = 0; i < vulkan_swapchain->image_count; i++) {
        assert(vulkan_swapchain->textures[i].backbuffer_data);

        vkDestroyImageView(vulkan_device->device, vulkan_swapchain->textures[i].image_view, nullptr);
        vkDestroySemaphore(vulkan_device->device, vulkan_swapchain->acquire_semaphores[i], nullptr);
        vkDestroySemaphore(vulkan_device->device, vulkan_swapchain->present_semaphores[i], nullptr);
        vkDestroyFence(vulkan_device->device, vulkan_swapchain->fences[i], nullptr);
        vkFreeCommandBuffers(vulkan_device->device, vulkan_swapchain->command_pool, 1, &vulkan_swapchain->command_buffers[i]);
        vkDestroySemaphore(vulkan_device->device, vulkan_swapchain->textures[i].backbuffer_data->timeline, nullptr);
        pfree(vulkan_swapchain->textures[i].backbuffer_data);
    }

    vkDestroyCommandPool(vulkan_device->device, vulkan_swapchain->command_pool, nullptr);
    vkDestroySwapchainKHR(vulkan_device->device, vulkan_swapchain->swapchain, nullptr);
    vkDestroySurfaceKHR(vk.instance, vulkan_swapchain->surface, nullptr);

    pfree(vulkan_swapchain->present_semaphores);
    pfree(vulkan_swapchain->acquire_semaphores);
    pfree(vulkan_swapchain->command_buffers);
    pfree(vulkan_swapchain->textures);
    pfree(vulkan_swapchain->fences);
    pfree(vulkan_swapchain);
}

RHITexture vk_next_backbuffer(RHISwapchain swapchain) {
    assert(swapchain);

    auto vulkan_swapchain = (Swapchain *) swapchain;

    u32 semaphore_index = vulkan_swapchain->semaphore_index;

    if (vkWaitForFences(vulkan_swapchain->device, 1, &vulkan_swapchain->fences[semaphore_index], true, UINT64_MAX) != VK_SUCCESS) {
        assert(false);
    }

    if (vkResetFences(vulkan_swapchain->device, 1, &vulkan_swapchain->fences[semaphore_index]) != VK_SUCCESS) {
        assert(false);
    }

    VkSemaphore acquire_semaphore = vulkan_swapchain->acquire_semaphores[vulkan_swapchain->semaphore_index];

    u32 index = 0;
    if (vkAcquireNextImageKHR(vulkan_swapchain->device, vulkan_swapchain->swapchain, UINT64_MAX, acquire_semaphore, nullptr, &index) != VK_SUCCESS) {
        assert(false);
    }

    // Assign semaphores from swapchain ringbuffer
    Texture *texture = &vulkan_swapchain->textures[index];
    assert(!texture->backbuffer_data->valid);
    texture->backbuffer_data->present_semaphore = vulkan_swapchain->present_semaphores[vulkan_swapchain->semaphore_index];
    texture->backbuffer_data->present_fence = vulkan_swapchain->fences[vulkan_swapchain->semaphore_index];
    texture->backbuffer_data->valid = true;
    texture->backbuffer_data->image_index = index;
    texture->backbuffer_data->command_buffer = vulkan_swapchain->command_buffers[vulkan_swapchain->semaphore_index];

    // Convert binary acquire semaphore to timeline semaphore for synchronization with command buffer submission
    VkSemaphoreSubmitInfo wait = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = acquire_semaphore,
    };
    texture->backbuffer_data->ready_value += 1;
    VkSemaphoreSubmitInfo signal = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = texture->backbuffer_data->timeline,
        .value = texture->backbuffer_data->ready_value,
    };
    VkSubmitInfo2 submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &wait,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &signal,
    };
    vkQueueSubmit2(vulkan_swapchain->queue, 1, &submit_info, nullptr);

    vulkan_swapchain->semaphore_index = (vulkan_swapchain->semaphore_index + 1) % vulkan_swapchain->image_count;

    return texture;
}

// The shit i have to do to implicitly sync present is dumb

void vk_present(RHISwapchain swapchain, RHITexture texture) {
    assert(swapchain);
    assert(texture);

    auto vulkan_swapchain = (Swapchain *) swapchain;
    auto vulkan_texture = (Texture *) texture;

    assert(vulkan_texture->backbuffer_data);
    assert(vulkan_texture->backbuffer_data->valid);

    // Convert timeline semaphore to binary semaphore for present
    VkCommandBufferSubmitInfo command_buffer_info = {};
    bool use_command_buffer = false;

    if (vulkan_texture->layout != VK_IMAGE_LAYOUT_PRESENT_SRC_KHR || vulkan_texture->queue_index != vulkan_swapchain->queue_family) {
        // For the sake of simplicity we manage the command buffer here
        if (vkResetCommandBuffer(vulkan_texture->backbuffer_data->command_buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS) {
            assert(false);
        }

        VkCommandBufferBeginInfo begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };
        if (vkBeginCommandBuffer(vulkan_texture->backbuffer_data->command_buffer, &begin_info) != VK_SUCCESS) {
            assert(false);
        }

        VkImageMemoryBarrier2 image_barrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = 0,
            .oldLayout = vulkan_texture->layout,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = vulkan_texture->queue_index,
            .dstQueueFamilyIndex = vulkan_swapchain->queue_family,
            .image = vulkan_texture->image,
            .subresourceRange = {
                .aspectMask = vulkan_texture->aspect_mask,
                .baseMipLevel = 0,
                .levelCount = vulkan_texture->mip_count,
                .baseArrayLayer = 0,
                .layerCount = vulkan_texture->layers
            },
        };
        if (vulkan_texture->layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            image_barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
            image_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }

        vulkan_texture->layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        vulkan_texture->queue_index = vulkan_swapchain->queue_family;

        VkDependencyInfo dep = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &image_barrier,
        };
        vkCmdPipelineBarrier2(vulkan_texture->backbuffer_data->command_buffer, &dep);

        if (vkEndCommandBuffer(vulkan_texture->backbuffer_data->command_buffer) != VK_SUCCESS) {
            assert(false);
        }

        use_command_buffer = true;
        command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        command_buffer_info.commandBuffer = vulkan_texture->backbuffer_data->command_buffer;
    }

    VkSemaphoreSubmitInfo wait = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = vulkan_texture->backbuffer_data->timeline,
        .value = vulkan_texture->backbuffer_data->ready_value
    };

    VkSemaphoreSubmitInfo signal = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = vulkan_texture->backbuffer_data->present_semaphore,
    };

    VkSubmitInfo2 submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &wait,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &signal,
    };

    if (use_command_buffer) {
        submit_info.commandBufferInfoCount = 1;
        submit_info.pCommandBufferInfos = &command_buffer_info;
    }

    if (vkQueueSubmit2(vulkan_swapchain->queue, 1, &submit_info, vulkan_texture->backbuffer_data->present_fence) != VK_SUCCESS) {
        assert(false);
    }

    // Present
    VkResult result = VK_SUCCESS;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &vulkan_texture->backbuffer_data->present_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &vulkan_swapchain->swapchain,
        .pImageIndices = &vulkan_texture->backbuffer_data->image_index,
        .pResults = &result,
    };

    if (vkQueuePresentKHR(vulkan_swapchain->queue, &present_info) != VK_SUCCESS) {
        assert(false);
    }

    assert(result == VK_SUCCESS);

    vulkan_texture->backbuffer_data->present_semaphore = nullptr;
    vulkan_texture->backbuffer_data->command_buffer = nullptr;
    vulkan_texture->backbuffer_data->valid = false;
}

// Semaphores
RHISemaphore vk_create_semaphore(RHIDevice device, u64 init_value) {
    assert(device);

    auto vulkan_device = (Device *) device;

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

    auto semaphore = (Semaphore *) palloc(sizeof(Semaphore));

    semaphore->semaphore = sem;

    return semaphore;
}

void vk_wait_semaphore(RHIDevice device, RHISemaphore sem, u64 value) {
    assert(device);
    assert(sem);

    auto vulkan_device = (Device *) device;
    auto semaphore = (Semaphore *) sem;

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

    auto vulkan_device = (Device *) device;
    auto semaphore = (Semaphore *) sem;

    vkDestroySemaphore(vulkan_device->device, semaphore->semaphore, nullptr);
    pfree(semaphore);
}

// Commands
void vk_mem_copy(RHICommandBuffer cb, void *dst_gpu, void *src_gpu, u64 size) {
    assert(cb);
    assert(size);

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, dst_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, src_gpu, GPU);

    BufferOffset dst = get_buffer_offset_gpu(command_buffer->queue->device, dst_gpu);
    BufferOffset src = get_buffer_offset_gpu(command_buffer->queue->device, src_gpu);

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

    auto command_buffer = (CommandBuffer *) cb;
    auto tex = (Texture *) texture;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, src_gpu, GPU);

    BufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, src_gpu);

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
    assert(texture);

    auto command_buffer = (CommandBuffer *) cb;
    auto tex = (Texture *) texture;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, dst_gpu, GPU);

    BufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, dst_gpu);

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
    assert(size);

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, ptr_gpu, GPU);

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

    auto command_buffer = (CommandBuffer *) cb;

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

    auto command_buffer = (CommandBuffer *) cb;
    auto semaphore = (Semaphore *) sem;

    VkSemaphoreSubmitInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore->semaphore,
        .value = value,
        .stageMask = vk_get_pipeline_stage(before),
    };
    rhi::dyn_array_push_back(&command_buffer->signals, semaphore_info, vk.alloc);
}

void vk_wait_before(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value) {
    assert(cb);
    assert(sem);

    auto command_buffer = (CommandBuffer *) cb;
    auto semaphore = (Semaphore *) sem;

    VkSemaphoreSubmitInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = semaphore->semaphore,
        .value = value,
        .stageMask = vk_get_pipeline_stage(after),
    };
    rhi::dyn_array_push_back(&command_buffer->waits, semaphore_info, vk.alloc);
}

void vk_set_pipeline(RHICommandBuffer cb, RHIPipeline pipeline) {
    assert(cb);
    assert(pipeline);

    auto command_buffer = (CommandBuffer *) cb;
    auto p = (Pipeline *) pipeline;

    if (command_buffer == nullptr || p == nullptr) {
        return;
    }

    vkCmdBindPipeline(command_buffer->command_buffer, p->bind_point, p->pipeline);
}

void vk_set_depth_stencil_state(RHICommandBuffer cb, RHIDepthStencilState state) {
    assert(cb);
    assert(state);

    auto command_buffer = (CommandBuffer *) cb;
    auto depth_stencil = (DepthStencilState *) state;

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

    auto command_buffer = (CommandBuffer *) cb;
    auto blend = (BlendState *) state;

    VkBool32 enable = true;
    vkCmdSetColorBlendEnableEXT(command_buffer->command_buffer, 0, 1, &enable);

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

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, data_gpu, GPU);

    ComputePushConstants pc = {
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

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, grid_dimensions_gpu, GPU);

    ComputePushConstants pc = {
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

    BufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, grid_dimensions_gpu);
    vkCmdDispatchIndirect(command_buffer->command_buffer, offset.buffer, offset.offset);
}

void vk_begin_render_pass(RHICommandBuffer cb, RHIRenderPassDesc *desc) {
    assert(cb);
    assert(desc);

    defer {
        vk.temp_alloc->reset(vk.temp_alloc->user_data);
    };

    auto command_buffer = (CommandBuffer *) cb;

    VkRect2D render_area = {};
    render_area.extent.width = UINT32_MAX;
    render_area.extent.height = UINT32_MAX;

    u32 color_attachment_count = desc->color_attachment_count;
    rhi::Array<VkRenderingAttachmentInfo> color_attachments = rhi::array<VkRenderingAttachmentInfo>(color_attachment_count, vk.temp_alloc);

    for (u32 i = 0; i < desc->color_attachment_count; i++) {
        auto texture = (Texture *) desc->color_attachments[i].texture;

        assert(texture);

        // track swapchain backbuffer target
        if (texture->backbuffer_data) {
            assert(!command_buffer->backbuffer || command_buffer->backbuffer == texture);
            assert(texture->backbuffer_data->valid);
            command_buffer->backbuffer = texture;
        }

        if (texture->layout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL || texture->queue_index != command_buffer->queue->family) {
            VkImageMemoryBarrier2 image_barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                .srcAccessMask = 0,
                .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = texture->layout,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .srcQueueFamilyIndex = texture->queue_index,
                .dstQueueFamilyIndex = command_buffer->queue->family,
                .image = texture->image,
                .subresourceRange = {
                    .aspectMask = texture->aspect_mask,
                    .baseMipLevel = 0,
                    .levelCount = texture->mip_count,
                    .baseArrayLayer = 0,
                    .layerCount = texture->layers
                },
            };
            texture->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            texture->queue_index = command_buffer->queue->family;
            VkDependencyInfo dep = {
                .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                .imageMemoryBarrierCount = 1,
                .pImageMemoryBarriers = &image_barrier,
            };

            vkCmdPipelineBarrier2(command_buffer->command_buffer, &dep);
        }

        color_attachments.data[i] = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = texture->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE
        };

        if (desc->color_attachments[i].clear) {
            color_attachments.data[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            color_attachments.data[i].clearValue.color.float32[0] = desc->color_attachments[i].clear_value.color[0];
            color_attachments.data[i].clearValue.color.float32[1] = desc->color_attachments[i].clear_value.color[1];
            color_attachments.data[i].clearValue.color.float32[2] = desc->color_attachments[i].clear_value.color[2];
            color_attachments.data[i].clearValue.color.float32[3] = desc->color_attachments[i].clear_value.color[3];
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
        .colorAttachmentCount = (u32) color_attachments.count,
        .pColorAttachments = color_attachments.data,
    };

    auto depth_stencil = (Texture *) desc->depth_stencil_attachment.texture;
    
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

    auto command_buffer = (CommandBuffer *) cb;

    vkCmdEndRendering(command_buffer->command_buffer);
}

void vk_draw_indexed_instanced(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count) {
    assert(cb);

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, vertex_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, pixel_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, indices_gpu, GPU);

    RasterPushConstants pc = {
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

    BufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, indices_gpu);
    vkCmdBindIndexBuffer(command_buffer->command_buffer, offset.buffer, offset.offset, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(command_buffer->command_buffer, index_count, instance_count, 0, 0, 0);
}

void vk_draw_indexed_instanced_indirect(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, void *args_gpu) {
    assert(cb);

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, vertex_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, pixel_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, indices_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, args_gpu, GPU);

    RasterPushConstants pc = {
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

    BufferOffset index_offset = get_buffer_offset_gpu(command_buffer->queue->device, indices_gpu);
    vkCmdBindIndexBuffer(command_buffer->command_buffer, index_offset.buffer, index_offset.offset, VK_INDEX_TYPE_UINT16);

    BufferOffset indirect_offset = get_buffer_offset_gpu(command_buffer->queue->device, args_gpu);
    vkCmdDrawIndexedIndirect(command_buffer->command_buffer, indirect_offset.buffer, indirect_offset.offset, 1, 0);
}

void vk_draw_indexed_instanced_indirect_multi(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *args_gpu, void *draw_count_gpu, u32 stride) {
    assert(cb);

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, vertex_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, pixel_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, args_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, draw_count_gpu, GPU);

    RasterPushConstants pc = {
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

    BufferOffset indirect_offset = get_buffer_offset_gpu(command_buffer->queue->device, args_gpu);
    BufferOffset count_offset = get_buffer_offset_gpu(command_buffer->queue->device, draw_count_gpu);
    vkCmdDrawIndexedIndirectCount(command_buffer->command_buffer, indirect_offset.buffer, indirect_offset.offset, count_offset.buffer, count_offset.offset, 0, stride);
}

void vk_draw_meshlets(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, u32 dim[3]) {
    assert(cb);

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, meshlet_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, pixel_data_gpu, GPU);

    MeshPushConstants pc = {
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

    auto command_buffer = (CommandBuffer *) cb;

    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, meshlet_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, pixel_data_gpu, GPU);
    ASSERT_RESOURCE_IS_STRICT(command_buffer->queue->device, dim_gpu, GPU);

    MeshPushConstants pc = {
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

    BufferOffset offset = get_buffer_offset_gpu(command_buffer->queue->device, dim_gpu);
    vkCmdDrawMeshTasksIndirectEXT(command_buffer->command_buffer, offset.buffer, offset.offset, 1, 0);
}

bool vulkan_init(RHI *rhi, RHIAllocator *alloc, RHIAllocator *temp_alloc) {
    loader_init();

    if (alloc && alloc->alloc && alloc->free && alloc->reset) {
        vk.alloc = alloc;
    } else if (alloc) {
        SK_LOG_WARN("vulkan_init alloc incomplete");
        assert(false);
    } else {
        RHIAllocator *default_alloc = (RHIAllocator *) malloc(sizeof(RHIAllocator));
        if (!default_alloc) {
            SK_LOG_ERROR("vulkan_init malloc failed");
            return false;
        }
        default_alloc->alloc = [](u64 size, void *user_data) -> void * {
            void *ptr = malloc(size);
            assert(ptr);
            memset(ptr, 0, size);
            return ptr;
        };
        default_alloc->free = [](void *ptr, void *user_data) {
            if (!ptr) {
                return;
            }
            free(ptr);
        };

        TempAllocatorData *data = (TempAllocatorData *) malloc(sizeof(TempAllocatorData));
        data->capacity = 1024 * 1024;
        data->base = (u8 *) malloc(data->capacity);
        data->curr = data->base;

        RHIAllocator *default_temp_allocator = (RHIAllocator *) malloc(sizeof(RHIAllocator));
        if (!default_temp_allocator) {
            SK_LOG_ERROR("vulkan_init malloc failed");
            free(default_alloc);
            return false;
        }
        default_temp_allocator->alloc = [](u64 size, void *user_data) -> void * {
            TempAllocatorData *data = (TempAllocatorData *) user_data;
            assert(data->curr + size <= data->base + data->capacity);
            void *ret = data->curr;
            data->curr += size;
            memset(ret, 0, size);
            return ret;
        };
        default_temp_allocator->reset = [](void *user_data) {
            TempAllocatorData *data = (TempAllocatorData *) user_data;
            data->curr = data->base;
        };
        default_temp_allocator->user_data = data;
    }

    if (temp_alloc && temp_alloc->alloc && temp_alloc->free && temp_alloc->reset) {
        vk.temp_alloc = temp_alloc;
    } else if (temp_alloc) {
        SK_LOG_WARN("vulkan_init temp_alloc incomplete");
        assert(false);
    }

    defer {
        temp_alloc->reset(temp_alloc->user_data);
    };

    bool extensions_supported = true;
    u32 layer_count = 0;

    if (vkEnumerateInstanceLayerProperties(&layer_count, nullptr) != VK_SUCCESS) {
        assert(false);
    }
    rhi::Array<VkLayerProperties> layers = rhi::array<VkLayerProperties>(layer_count, vk.temp_alloc);
    if (vkEnumerateInstanceLayerProperties(&layer_count, layers.data) != VK_SUCCESS) {
        SK_LOG_ERROR("vulkan_init vkEnumerateInstanceLayerProperties failed");
        return false;
    }

    for (u32 i = 0; i < validation_layer_count; i++) {
        bool layer_found = false;

        for (u32 k = 0; k < layers.count; k++) {
            if (strcmp(validation_layers[i], layers.data[k].layerName) == 0) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            extensions_supported = false;
            break;
        }
    }

    if (!extensions_supported) {
        SK_LOG_ERROR("vulkan_init !extensions_supported");
        return false;
    }

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
        debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_info.pfnUserCallback = debug_messenger;

        instance_info.pNext = &debug_info;
    }

    u32 extension_count = platform_get_required_extension_count();
    if (enable_validation) {
        extension_count += 1;
    }
    rhi::Array<const char *> extensions = rhi::array<const char *>(extension_count, vk.temp_alloc);
    platform_get_required_extensions(extensions.data);
    if (enable_validation) {
        extensions.data[extension_count - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    instance_info.enabledExtensionCount = extensions.count;
    instance_info.ppEnabledExtensionNames = extensions.data;

    if (vkCreateInstance(&instance_info, nullptr, &vk.instance) != VK_SUCCESS) {
        SK_LOG_ERROR("vulkan_init vkCreateInstance failed");
        return false;
    }

    // load instance-level extension symbols
    if (!load_instance_functions(vk.instance)) {
        SK_LOG_ERROR("vulkan_init load_instance_functions failed");
        return false;
    }

    if (vkCreateDebugUtilsMessengerEXT(vk.instance, &debug_info, nullptr, &vk.debug_messenger) != VK_SUCCESS) {
        SK_LOG_ERROR("vulkan_init vkCreateDebugUtilsMessengerEXT failed");
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
    if (vk.instance) {
        if (vk.debug_messenger) {
            vkDestroyDebugUtilsMessengerEXT(vk.instance, vk.debug_messenger, nullptr);
        }
        vkDestroyInstance(vk.instance, nullptr);
        loader_shutdown();
    }
}