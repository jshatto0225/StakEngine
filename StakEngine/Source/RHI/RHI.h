#ifndef RHI_H
#define RHI_H

// RHI - Rendering Hardware Interface
// C interface for interacting with the GPU, implemented by different backends (Vulkan, DirectX, etc)

#ifndef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef float f32;
typedef double f64;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef u8 b8;
typedef u16 b16;
typedef u32 b32;

#define _nullable

//
// Allocator requirements:
//   - alloc: Allocates a block of memory of the given size.
//       Returns a pointer to the allocated memory, or nullptr if allocation fails.
//       The allocated memory should always be set to 0.
//   - realloc: Changes the size of the memory block pointed to by ptr to new_size bytes.
//       Returns a pointer to the newly allocated memory, which may be the same as ptr or a new location.
//       If allocation fails, returns nullptr and leaves the original block unchanged.
//   - free: Frees the memory block pointed to by *ptr and sets *ptr to nullptr. If *ptr is already nullptr, does nothing.
//   reset is only valid for temp_alloc in rhi_init.
//   free is only valid for alloc in rhi_init.
//   One of reset/free must exist.
//   The lifetime of allocators passed to rhi_init must be greater than the lifetime of rhi_init->rhi_shutdown.
//   Allocators should be reset/freed by the user after rhi_shutdown, the rhi will not do this automatically.
//   
struct RHIAllocator {
    void *(*alloc)(u64 bytes, void *user_data);
    void (_nullable *free)(_nullable void *ptr, void *user_data);
    void (_nullable *reset)(void *user_data);

    void *user_data;
};

enum RHISeverity {
    RHI_SEVERITY_TRACE,
    RHI_SEVERITY_INFO,
    RHI_SEVERITY_WARN,
    RHI_SEVERITY_ERROR,
    RHI_SEVERITY_CRITICAL,
};

typedef void (*RHILog)(const char *message, RHISeverity severity);

#define RHI_HANDLE(name) typedef void *RHI##name;

// Opaque handles
RHI_HANDLE(Pipeline);
RHI_HANDLE(Texture);
RHI_HANDLE(DepthStencilState);
RHI_HANDLE(BlendState);
RHI_HANDLE(Queue);
RHI_HANDLE(CommandBuffer);
RHI_HANDLE(Semaphore);
RHI_HANDLE(Device);
RHI_HANDLE(Swapchain);

// Enums
enum RHIMemoryType {
    RHI_MEMORY_TYPE_DEFAULT,
    RHI_MEMORY_TYPE_GPU,
    RHI_MEMORY_TYPE_READBACK
};

enum RHICullMode {
    RHI_CULL_MODE_CCW,
    RHI_CULL_MODE_CW,
    RHI_CULL_MODE_ALL,
    RHI_CULL_MODE_NONE
};

enum RHIDepthFlags {
    RHI_DEPTH_READ = 0x1,
    RHI_DEPTH_WRITE = 0x2
};

enum RHIOp {
    RHI_OP_NEVER,
    RHI_OP_LESS,
    RHI_OP_EQUAL,
    RHI_OP_LESS_EQUAL,
    RHI_OP_GREATER,
    RHI_OP_NOT_EQUAL,
    RHI_OP_GREATER_EQUAL,
    RHI_OP_ALWAYS
};

enum RHIBlendOp {
    RHI_BLEND_OP_ADD,
    RHI_BLEND_OP_SUBTRACT,
    RHI_BLEND_OP_REV_SUBTRACT,
    RHI_BLEND_OP_MIN,
    RHI_BLEND_OP_MAX
};

enum RHIBlendFactor {
    RHI_BLEND_FACTOR_ZERO,
    RHI_BLEND_FACTOR_ONE,
    RHI_BLEND_FACTOR_SRC_COLOR,
    RHI_BLEND_FACTOR_DST_COLOR,
    RHI_BLEND_FACTOR_SRC_ALPHA
};

enum RHITopology {
    RHI_TOPOLOGY_TRIANGLE_LIST,
    RHI_TOPOLOGY_TRIANGLE_STRIP,
    RHI_TOPOLOGY_TRIANGLE_FAN
};

enum RHITextureType {
    RHI_TEXTURE_TYPE_1D,
    RHI_TEXTURE_TYPE_2D,
    RHI_TEXTURE_TYPE_3D,
    RHI_TEXTURE_TYPE_CUBE,
    RHI_TEXTURE_TYPE_2D_ARRAY,
    RHI_TEXTURE_TYPE_CUBE_ARRAY
};

enum RHIFormat {
    RHI_FORMAT_NONE,
    RHI_FORMAT_RGBA8_UNORM,
    RHI_FORMAT_RGBA8_SRGB,
    RHI_FORMAT_D32_FLOAT,
    RHI_FORMAT_RGB10_A2_UNORM,
};

enum RHIUsageFlags {
    RHI_USAGE_SAMPLED,
    RHI_USAGE_STORAGE,
    RHI_USAGE_COLOR_ATTACHMENT,
    RHI_USAGE_DEPTH_STENCIL_ATTACHMENT
};

enum RHIPipelineStage {
    RHI_PIPELINE_STAGE_TRANSFER,
    RHI_PIPELINE_STAGE_COMPUTE,
    RHI_PIPELINE_STAGE_RASTER_COLOR_OUT,
    RHI_PIPELINE_STAGE_PIXEL_SHADER,
    RHI_PIPELINE_STAGE_VERTEX_SHADER
};

enum RHIHazardFlags {
    RHI_HAZARD_NONE = 0,
    RHI_HAZARD_DRAW_ARGUMENTS = 0x1,
    RHI_HAZARD_DESCRIPTORS = 0x2,
    RHI_HAZARD_DEPTH_STENCIL = 0x4
};

enum RHISignal {
    RHI_SIGNAL_ATOMIC_SET,
    RHI_SIGNAL_ATOMIC_MAX,
    RHI_SIGNAL_ATOMIC_OR
};

enum RHIStencilOp {
    RHI_STENCIL_OP_KEEP,
    RHI_STENCIL_OP_ZERO,
    RHI_STENCIL_OP_REPLACE,
    RHI_STENCIL_OP_INCREMENT_AND_CLAMP,
    RHI_STENCIL_OP_DECREMENT_AND_CLAMP,
    RHI_STENCIL_OP_INVERT,
    RHI_STENCIL_OP_INCREMENT_AND_WRAP,
    RHI_STENCIL_OP_DECREMENT_AND_WRAP,
};

enum RHIPlatform {
    RHI_PLATFORM_WIN32,
};

// Structs
struct RHIStencil {
    RHIOp test;
    RHIStencilOp fail_op;
    RHIStencilOp pass_op;
    RHIStencilOp depth_fail_op;
    u8 reference;
};

struct RHIDepthStencilDesc {
    RHIDepthFlags depth_mode;
    RHIOp depth_test;
    float depth_bias;
    float depth_bias_slope_factor;
    float depth_bias_clamp;
    u8 stencil_read_mask;
    u8 stencil_write_mask;
    RHIStencil stencil_front;
    RHIStencil stencil_back;
};

struct RHIBlendDesc {
    RHIBlendOp color_op;
    RHIBlendFactor src_color_factor;
    RHIBlendFactor dst_color_factor;
    RHIBlendOp alpha_op;
    RHIBlendFactor src_alpha_factor;
    RHIBlendFactor dst_alpha_factor;
    u8 color_write_mask;
};

struct RHIColorTarget {
    RHIFormat format;
    u8 write_mask;
};

struct RHIRasterDesc {
    RHITopology topology;
    RHICullMode cull;
    bool alpha_to_coverage;
    bool support_dual_source_blending;
    u8 sample_count;
    RHIFormat depth_format;
    RHIFormat stencil_format;
    RHIColorTarget *color_targets;
    u32 color_target_count;
    RHIBlendDesc *blend_state; // optional embedded blend state
};

struct RHITextureDesc {
    RHITextureType type;
    u32 dimensions[3];
    u32 mip_count;
    u32 layer_count;
    u32 sample_count;
    RHIFormat format;
    RHIUsageFlags usage;
};

struct RHIViewDesc {
    RHIFormat format;
    u8 base_mip;
    u8 mip_count;
    u16 base_layer;
    u16 layer_count;
};

struct RHITextureSizeAlign {
    size_t size;
    size_t align;
};

struct RHITextureDescriptor {
    u64 data[4];
};

enum RHIClearValueType {
    RHI_CLEAR_VALUE_TYPE_COLOR,
    RHI_CLEAR_VALUE_TYPE_DEPTH_STENCIL
};

struct RHIClearValue {
    RHIClearValueType type;
    union {
        f32 color[4];
        struct {
            float depth;
            u8 stencil;
        } depth_stencil;
    };
};

struct RHIRenderPassAttachment {
    RHITexture texture;
    bool clear;
    RHIClearValue clear_value;
};

struct RHIRenderPassDesc {
    RHIRenderPassAttachment *color_attachments;
    u32 color_attachment_count;
    RHIRenderPassAttachment depth_stencil_attachment;
};

enum RHIQueueCapability {
    RHI_QUEUE_GRAPHICS = 1 << 0,
    RHI_QUEUE_COMPUTE = 1 << 1,
    RHI_QUEUE_TRANSFER = 1 << 2,
};

struct RHIQueueRequest {
    u32 capabilities;
    u32 count;
};

struct RHIDeviceDesc {
    RHIQueueRequest *queues;
    u32 queue_count;
};

struct RHIQueueDesc {
    u32 capabilities;
    u32 index;
};

struct RHISwapchainDesc {
    RHIPlatform platform;

    u32 width;
    u32 height;

    RHIFormat format;

    u32 image_count;

    bool vsync;

    void *window;
    _nullable void *display;
};

struct RHI {
    // Memory
    void *(*alloc)(RHIDevice device, u64 bytes, RHIMemoryType memory);
    void (*free)(RHIDevice device, _nullable void *ptr);
    void *(*host_to_device_pointer)(RHIDevice device, void *ptr);

    // Device
    RHIDevice (*create_device)(RHIDeviceDesc *desc);
    void (*destroy_device)(RHIDevice device);
    void (*device_wait_idle)(RHIDevice device);

    // Swapchain
    RHISwapchain (*create_swapchain)(RHIDevice device, RHISwapchainDesc *desc);
    void (*destroy_swapchain)(RHIDevice device, RHISwapchain swapchain);
    RHITexture (*next_backbuffer)(RHISwapchain swapchain);
    void (*present)(RHISwapchain swapchain, RHITexture texture);

    // Textures
    RHITextureSizeAlign (*texture_size_align)(RHIDevice device, RHITextureDesc *desc);
    RHITexture (*create_texture)(RHIDevice device, RHITextureDesc *desc, void *ptr_gpu);
    void (*destroy_texture)(RHIDevice device, RHITexture texture);
    RHITextureDescriptor (*texture_view_descriptor)(RHIDevice device, RHITexture texture, RHIViewDesc *desc);
    RHITextureDescriptor (*rw_texture_view_descriptor)(RHIDevice device, RHITexture texture, RHIViewDesc *desc);

    // Pipelines
    RHIPipeline (*create_compute_pipeline)(RHIDevice device, u8 *compute_ir, u32 ir_size);
    RHIPipeline (*create_graphics_pipeline)(RHIDevice device, u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc);
    RHIPipeline (*create_graphics_meshlet_pipeline)(RHIDevice device, u8 *meshlet_ir, u32 meshlet_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc);
    void (*destroy_pipeline)(RHIDevice device, RHIPipeline pipeline);

    // State objects
    RHIDepthStencilState (*create_depth_stencil_state)(RHIDevice device, RHIDepthStencilDesc *desc);
    RHIBlendState (*create_blend_state)(RHIDevice device, RHIBlendDesc *desc);
    void (*free_depth_stencil_state)(RHIDevice device, RHIDepthStencilState state);
    void (*free_blend_state)(RHIDevice device, RHIBlendState state);

    // Queue
    RHIQueue (*get_queue)(RHIDevice device, RHIQueueDesc *desc);
    RHICommandBuffer (*start_command_recording)(RHIQueue queue);
    void (*submit)(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count, RHISemaphore semaphore, u64 semaphore_value);

    // Semaphores
    RHISemaphore (*create_semaphore)(RHIDevice device, u64 init_value);
    void (*wait_semaphore)(RHIDevice device, RHISemaphore semaphore, u64 value);
    void (*destroy_semaphore)(RHIDevice device, RHISemaphore semaphore);

    // Commands
    void (*mem_copy)(RHICommandBuffer cb, void *dest_gpu, void *src_gpu, u64 size);
    void (*copy_to_texture)(RHICommandBuffer cb, RHITexture texture, void *src_gpu);
    void (*copy_from_texture)(RHICommandBuffer cb, void *dest_gpu, RHITexture texture);

    void (*set_active_texture_heap_ptr)(RHICommandBuffer cb, void *ptr_gpu, u64 size);

    void (*barrier)(RHICommandBuffer cb, RHIPipelineStage before, RHIPipelineStage after, RHIHazardFlags hazards);
    void (*signal_after)(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value);
    void (*wait_before)(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value);

    void (*set_pipeline)(RHICommandBuffer cb, RHIPipeline pipeline);
    void (*set_depth_stencil_state)(RHICommandBuffer cb, RHIDepthStencilState state);
    void (*set_blend_state)(RHICommandBuffer cb, RHIBlendState state);

    void (*dispatch)(RHICommandBuffer cb, void *dataGpu, u32 grid_dimensions[3]);
    void (*dispatch_indirect)(RHICommandBuffer cb, void *dataGpu, void *grid_dimensions_gpu);

    void (*begin_render_pass)(RHICommandBuffer cb, RHIRenderPassDesc *desc);
    void (*end_render_pass)(RHICommandBuffer cb);

    void (*draw_indexed_instanced)(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count);
    void (*draw_indexed_instanced_indirect)(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, void *args_gpu);
    void (*draw_indexed_instanced_indirect_multi)(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *args_gpu, void *draw_count_gpu, u32 stride);

    void (*draw_meshlets)(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, u32 dim[3]);
    void (*draw_meshlets_indirect)(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, void *dim_gpu);
};

// allocation_callbacks can be nullptr, in which case the RHI will use a default allocator that uses malloc/free. 
// If allocation_callbacks is not nullptr, the RHI will use the provided allocator for all memory allocations and deallocations. 
// The user_data field of the allocation callbacks can be used to pass additional information to the allocator functions, such as a pointer to a custom allocator object or a logging function. 
// The RHI will not modify the allocation callbacks or the user_data pointer, so it is the responsibility of the caller to ensure that they remain valid for the lifetime of the RHI.
// The allocation_callbacks cannot be changed after initialization.
bool rhi_init(_nullable RHIAllocator *alloc, _nullable RHIAllocator *temp_alloc, _nullable RHILog log);
void rhi_shutdown();

// Memory
void *rhi_alloc(RHIDevice device, u64 bytes, RHIMemoryType memory);
void rhi_free(RHIDevice device, _nullable void *ptr);
void *rhi_host_to_device_pointer(RHIDevice device, void *ptr);

// Device
RHIDevice rhi_create_device(RHIDeviceDesc *desc);
void rhi_destroy_device(RHIDevice device);
void rhi_device_wait_idle(RHIDevice device);

// Swapchain
RHISwapchain rhi_create_swapchain(RHIDevice device, RHISwapchainDesc *desc);
void rhi_destroy_swapchain(RHIDevice device, RHISwapchain swapchain);
RHITexture rhi_next_backbuffer(RHISwapchain swapchain);
void rhi_present(RHISwapchain swapchain, RHITexture texture);

// Textures
RHITextureSizeAlign rhi_texture_size_align(RHIDevice device, RHITextureDesc *desc);
RHITexture rhi_create_texture(RHIDevice device, RHITextureDesc *desc, void *ptr_gpu);
void rhi_destroy_texture(RHIDevice device, RHITexture texture);
RHITextureDescriptor rhi_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc);
RHITextureDescriptor rhi_rw_texture_view_descriptor(RHIDevice device, RHITexture texture, RHIViewDesc *desc);

// Pipelines
RHIPipeline rhi_create_compute_pipeline(RHIDevice device, u8 *compute_ir, u32 ir_size);
RHIPipeline rhi_create_graphics_pipeline(RHIDevice device, u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc);
RHIPipeline rhi_create_graphics_meshlet_pipeline(RHIDevice device, u8 *meshlet_ir, u32 meshlet_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc *desc);
void rhi_destroy_pipeline(RHIDevice device, RHIPipeline pipeline);

// State objects
RHIDepthStencilState rhi_create_depth_stencil_state(RHIDevice device, RHIDepthStencilDesc *desc);
RHIBlendState rhi_create_blend_state(RHIDevice device, RHIBlendDesc *desc);
void rhi_free_depth_stencil_state(RHIDevice device, RHIDepthStencilState state);
void rhi_free_blend_state(RHIDevice device, RHIBlendState state);

// Queue
RHIQueue rhi_get_queue(RHIDevice device, RHIQueueDesc *desc);
RHICommandBuffer rhi_start_command_recording(RHIQueue queue);
void rhi_submit(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count, RHISemaphore semaphore, u64 semaphore_value);

// Semaphores
RHISemaphore rhi_create_semaphore(RHIDevice device, u64 init_value);
void rhi_wait_semaphore(RHIDevice device, RHISemaphore semaphore, u64 value);
void rhi_destroy_semaphore(RHIDevice device, RHISemaphore semaphore);

// Commands
void rhi_mem_copy(RHICommandBuffer cb, void *dest_gpu, void *src_gpu, u64 size);
void rhi_copy_to_texture(RHICommandBuffer cb, RHITexture texture, void *src_gpu);
void rhi_copy_from_texture(RHICommandBuffer cb, void *dest_gpu, RHITexture texture);

void rhi_set_active_texture_heap_ptr(RHICommandBuffer cb, void *ptr_gpu, u64 size);

void rhi_barrier(RHICommandBuffer cb, RHIPipelineStage before, RHIPipelineStage after, RHIHazardFlags hazards);
void rhi_signal_after(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value);
void rhi_wait_before(RHICommandBuffer cb, RHIPipelineStage after, RHISemaphore sem, u64 value);

void rhi_set_pipeline(RHICommandBuffer cb, RHIPipeline pipeline);
void rhi_set_depth_stencil_state(RHICommandBuffer cb, RHIDepthStencilState state);
void rhi_set_blend_state(RHICommandBuffer cb, RHIBlendState state);

void rhi_dispatch(RHICommandBuffer cb, void *dataGpu, u32 grid_dimensions[3]);
void rhi_dispatch_indirect(RHICommandBuffer cb, void *dataGpu, void *grid_dimensions_gpu);

void rhi_begin_render_pass(RHICommandBuffer cb, RHIRenderPassDesc *desc);
void rhi_end_render_pass(RHICommandBuffer cb);

void rhi_draw_indexed_instanced(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count);
void rhi_draw_indexed_instanced_indirect(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, void *args_gpu);
void rhi_draw_indexed_instanced_indirect_multi(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *args_gpu, void *draw_count_gpu, u32 stride);

void rhi_draw_meshlets(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, u32 dim[3]);
void rhi_draw_meshlets_indirect(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, void *dim_gpu);

#ifndef __cplusplus
}
#endif

#endif // RHI_H