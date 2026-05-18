#ifndef SK_H
#define SK_H

// TODO: Add gpu enumeration and selection before device creation.

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
//   reset is only valid for temp_alloc in sk_init.
//   free is only valid for alloc in sk_init.
//   One of reset/free must exist.
//   The lifetime of allocators passed to sk_init must be greater than the lifetime of sk_init->sk_shutdown.
//   Allocators should be reset/freed by the user after sk_shutdown, the sk will not do this automatically.
//   
struct SKAllocator {
    void *(*alloc)(u64 bytes, void *user_data);
    void (_nullable *free)(_nullable void *ptr, void *user_data);
    void (_nullable *reset)(void *user_data);

    void *user_data;
};

enum SKSeverity {
    SK_SEVERITY_TRACE,
    SK_SEVERITY_INFO,
    SK_SEVERITY_WARN,
    SK_SEVERITY_ERROR,
    SK_SEVERITY_CRITICAL,
};

typedef void (*SKLog)(const char *message, SKSeverity severity);

#define SK_HANDLE(name) typedef void *SK##name;

// Opaque handles
SK_HANDLE(Pipeline);
SK_HANDLE(Texture);
SK_HANDLE(DepthStencilState);
SK_HANDLE(BlendState);
SK_HANDLE(Queue);
SK_HANDLE(CommandBuffer);
SK_HANDLE(Semaphore);
SK_HANDLE(Device);
SK_HANDLE(Swapchain);

// Enums
enum SKMemoryType {
    SK_MEMORY_TYPE_DEFAULT,
    SK_MEMORY_TYPE_GPU,
    SK_MEMORY_TYPE_READBACK
};

enum SKCullMode {
    SK_CULL_MODE_CCW,
    SK_CULL_MODE_CW,
    SK_CULL_MODE_ALL,
    SK_CULL_MODE_NONE
};

enum SKDepthFlags {
    SK_DEPTH_READ = 0x1,
    SK_DEPTH_WRITE = 0x2
};

enum SKOp {
    SK_OP_NEVER,
    SK_OP_LESS,
    SK_OP_EQUAL,
    SK_OP_LESS_EQUAL,
    SK_OP_GREATER,
    SK_OP_NOT_EQUAL,
    SK_OP_GREATER_EQUAL,
    SK_OP_ALWAYS
};

enum SKBlendOp {
    SK_BLEND_OP_ADD,
    SK_BLEND_OP_SUBTRACT,
    SK_BLEND_OP_REV_SUBTRACT,
    SK_BLEND_OP_MIN,
    SK_BLEND_OP_MAX
};

enum SKBlendFactor {
    SK_BLEND_FACTOR_ZERO,
    SK_BLEND_FACTOR_ONE,
    SK_BLEND_FACTOR_SRC_COLOR,
    SK_BLEND_FACTOR_DST_COLOR,
    SK_BLEND_FACTOR_SRC_ALPHA
};

enum SKTopology {
    SK_TOPOLOGY_TRIANGLE_LIST,
    SK_TOPOLOGY_TRIANGLE_STRIP,
    SK_TOPOLOGY_TRIANGLE_FAN
};

enum SKTextureType {
    SK_TEXTURE_TYPE_1D,
    SK_TEXTURE_TYPE_2D,
    SK_TEXTURE_TYPE_3D,
    SK_TEXTURE_TYPE_CUBE,
    SK_TEXTURE_TYPE_2D_ARRAY,
    SK_TEXTURE_TYPE_CUBE_ARRAY
};

enum SKFormat {
    SK_FORMAT_NONE,
    SK_FORMAT_RGBA8_UNORM,
    SK_FORMAT_RGBA8_SRGB,
    SK_FORMAT_D32_FLOAT,
    SK_FORMAT_RGB10_A2_UNORM,
};

enum SKUsageFlags {
    SK_USAGE_SAMPLED,
    SK_USAGE_STORAGE,
    SK_USAGE_COLOR_ATTACHMENT,
    SK_USAGE_DEPTH_STENCIL_ATTACHMENT
};

enum SKPipelineStage {
    SK_PIPELINE_STAGE_TRANSFER,
    SK_PIPELINE_STAGE_COMPUTE,
    SK_PIPELINE_STAGE_RASTER_COLOR_OUT,
    SK_PIPELINE_STAGE_PIXEL_SHADER,
    SK_PIPELINE_STAGE_VERTEX_SHADER
};

enum SKHazardFlags {
    SK_HAZARD_NONE = 0,
    SK_HAZARD_DRAW_ARGUMENTS = 0x1,
    SK_HAZARD_DESCRIPTORS = 0x2,
    SK_HAZARD_DEPTH_STENCIL = 0x4
};

enum SKSignal {
    SK_SIGNAL_ATOMIC_SET,
    SK_SIGNAL_ATOMIC_MAX,
    SK_SIGNAL_ATOMIC_OR
};

enum SKStencilOp {
    SK_STENCIL_OP_KEEP,
    SK_STENCIL_OP_ZERO,
    SK_STENCIL_OP_REPLACE,
    SK_STENCIL_OP_INCREMENT_AND_CLAMP,
    SK_STENCIL_OP_DECREMENT_AND_CLAMP,
    SK_STENCIL_OP_INVERT,
    SK_STENCIL_OP_INCREMENT_AND_WRAP,
    SK_STENCIL_OP_DECREMENT_AND_WRAP,
};

enum SKPlatform {
    SK_PLATFORM_WIN32,
};

// Structs
struct SKStencil {
    SKOp test;
    SKStencilOp fail_op;
    SKStencilOp pass_op;
    SKStencilOp depth_fail_op;
    u8 reference;
};

struct SKDepthStencilDesc {
    SKDepthFlags depth_mode;
    SKOp depth_test;
    float depth_bias;
    float depth_bias_slope_factor;
    float depth_bias_clamp;
    u8 stencil_read_mask;
    u8 stencil_write_mask;
    SKStencil stencil_front;
    SKStencil stencil_back;
};

struct SKBlendDesc {
    SKBlendOp color_op;
    SKBlendFactor src_color_factor;
    SKBlendFactor dst_color_factor;
    SKBlendOp alpha_op;
    SKBlendFactor src_alpha_factor;
    SKBlendFactor dst_alpha_factor;
    u8 color_write_mask;
};

struct SKColorTarget {
    SKFormat format;
    u8 write_mask;
};

struct SKRasterDesc {
    SKTopology topology;
    SKCullMode cull;
    bool alpha_to_coverage;
    bool support_dual_source_blending;
    u8 sample_count;
    SKFormat depth_format;
    SKFormat stencil_format;
    SKColorTarget *color_targets;
    u32 color_target_count;
    SKBlendDesc *blend_state; // optional embedded blend state
};

struct SKTextureDesc {
    SKTextureType type;
    u32 dimensions[3];
    u32 mip_count;
    u32 layer_count;
    u32 sample_count;
    SKFormat format;
    SKUsageFlags usage;
};

struct SKViewDesc {
    SKFormat format;
    u8 base_mip;
    u8 mip_count;
    u16 base_layer;
    u16 layer_count;
};

struct SKTextureSizeAlign {
    size_t size;
    size_t align;
};

struct SKTextureDescriptor {
    u64 data[4];
};

enum SKClearValueType {
    SK_CLEAR_VALUE_TYPE_COLOR,
    SK_CLEAR_VALUE_TYPE_DEPTH_STENCIL
};

struct SKClearValue {
    SKClearValueType type;
    union {
        f32 color[4];
        struct {
            float depth;
            u8 stencil;
        } depth_stencil;
    };
};

struct SKRenderPassAttachment {
    SKTexture texture;
    bool clear;
    SKClearValue clear_value;
};

struct SKRenderPassDesc {
    SKRenderPassAttachment *color_attachments;
    u32 color_attachment_count;
    SKRenderPassAttachment depth_stencil_attachment;
};

enum SKQueueCapability {
    SK_QUEUE_GRAPHICS = 1 << 0,
    SK_QUEUE_COMPUTE = 1 << 1,
    SK_QUEUE_TRANSFER = 1 << 2,
};

struct SKQueueRequest {
    u32 capabilities;
    u32 count;
};

struct SKDeviceDesc {
    SKQueueRequest *queues;
    u32 queue_count;
};

struct SKQueueDesc {
    u32 capabilities;
    u32 index;
};

struct SKSwapchainDesc {
    SKPlatform platform;

    u32 width;
    u32 height;

    SKFormat format;

    u32 image_count;

    bool vsync;

    void *window;
    _nullable void *display;
};

// Setup/Teardown
typedef bool (*PFN_sk_init)     (_nullable SKAllocator *alloc, _nullable SKAllocator *temp_alloc, _nullable SKLog log);
typedef void (*PFN_sk_shutdown) ();

// Memory
typedef void *(*PFN_sk_alloc)              (SKDevice device, u64 bytes, SKMemoryType memory);
typedef void  (*PFN_sk_free)               (SKDevice device, _nullable void *ptr);
typedef void *(*PFN_sk_host_to_device_ptr) (SKDevice device, void *ptr);

// Device
typedef SKDevice (*PFN_sk_create_device)    (SKDeviceDesc *desc);
typedef void     (*PFN_sk_destroy_device)   (SKDevice device);
typedef void     (*PFN_sk_device_wait_idle) (SKDevice device);

// Swapchain
typedef SKSwapchain (*PFN_sk_create_swapchain)  (SKDevice device, SKSwapchainDesc *desc);
typedef void        (*PFN_sk_destroy_swapchain) (SKDevice device, SKSwapchain swapchain);
typedef SKTexture   (*PFN_sk_next_backbuffer)   (SKSwapchain swapchain);
typedef void        (*PFN_sk_present)           (SKSwapchain swapchain, SKTexture texture);

// Textures
typedef SKTextureSizeAlign  (*PFN_sk_texture_size_align)         (SKDevice device, SKTextureDesc *desc);
typedef SKTexture           (*PFN_sk_create_texture)             (SKDevice device, SKTextureDesc *desc, void *ptr_gpu);
typedef void                (*PFN_sk_destroy_texture)            (SKDevice device, SKTexture texture);
typedef SKTextureDescriptor (*PFN_sk_texture_view_descriptor)    (SKDevice device, SKTexture texture, SKViewDesc *desc);
typedef SKTextureDescriptor (*PFN_sk_rw_texture_view_descriptor) (SKDevice device, SKTexture texture, SKViewDesc *desc);

// Pipelines
typedef SKPipeline (*PFN_sk_create_compute_pipeline)          (SKDevice device, u8 *compute_ir, u32 ir_size);
typedef SKPipeline (*PFN_sk_create_graphics_pipeline)         (SKDevice device, u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, SKRasterDesc *desc);
typedef SKPipeline (*PFN_sk_create_graphics_meshlet_pipeline) (SKDevice device, u8 *meshlet_ir, u32 meshlet_ir_size, u8 *pixel_ir, u32 pixel_ir_size, SKRasterDesc *desc);
typedef void       (*PFN_sk_destroy_pipeline)                 (SKDevice device, SKPipeline pipeline);

// State objects
typedef SKDepthStencilState (*PFN_sk_create_depth_stencil_state) (SKDevice device, SKDepthStencilDesc *desc);
typedef SKBlendState        (*PFN_sk_create_blend_state)         (SKDevice device, SKBlendDesc *desc);
typedef void                (*PFN_sk_free_depth_stencil_state)   (SKDevice device, SKDepthStencilState state);
typedef void                (*PFN_sk_free_blend_state)           (SKDevice device, SKBlendState state);

// Queue
typedef SKQueue         (*PFN_sk_get_queue)               (SKDevice device, SKQueueDesc *desc);
typedef SKCommandBuffer (*PFN_sk_start_command_recording) (SKQueue queue);
typedef void            (*PFN_sk_submit)                  (SKQueue queue, SKCommandBuffer *command_buffers, u32 command_buffer_count, SKSemaphore semaphore, u64 semaphore_value);

// Semaphores
typedef SKSemaphore (*PFN_sk_create_semaphore)  (SKDevice device, u64 init_value);
typedef void        (*PFN_sk_wait_semaphore)    (SKDevice device, SKSemaphore semaphore, u64 value);
typedef void        (*PFN_sk_destroy_semaphore) (SKDevice device, SKSemaphore semaphore);

// Commands
typedef void (*PFN_sk_mem_copy)          (SKCommandBuffer cb, void *dest_gpu, void *src_gpu, u64 size);
typedef void (*PFN_sk_copy_to_texture)   (SKCommandBuffer cb, SKTexture texture, void *src_gpu);
typedef void (*PFN_sk_copy_from_texture) (SKCommandBuffer cb, void *dest_gpu, SKTexture texture);

typedef void (*PFN_sk_set_active_texture_heap_ptr) (SKCommandBuffer cb, void *ptr_gpu, u64 size);

typedef void (*PFN_sk_barrier)      (SKCommandBuffer cb, SKPipelineStage before, SKPipelineStage after, SKHazardFlags hazards);
typedef void (*PFN_sk_signal_after) (SKCommandBuffer cb, SKPipelineStage stage, SKSemaphore sem, u64 value);
typedef void (*PFN_sk_wait_before)  (SKCommandBuffer cb, SKPipelineStage stage, SKSemaphore sem, u64 value);

typedef void (*PFN_sk_signal_after2) (SKCommandBuffer cb, SKPipelineStage stage, void *gpu_ptr, u64 value, SKSignal signal);
typedef void (*PFN_sk_wait_before2)  (SKCommandBuffer cb, SKPipelineStage stage, void *gpu_ptr, u64 value, SKOp op, SKHazardFlags hazards, u64 mask);

typedef void (*PFN_sk_set_pipeline)            (SKCommandBuffer cb, SKPipeline pipeline);
typedef void (*PFN_sk_set_depth_stencil_state) (SKCommandBuffer cb, SKDepthStencilState state);
typedef void (*PFN_sk_set_blend_state)         (SKCommandBuffer cb, SKBlendState state);

typedef void (*PFN_sk_dispatch)          (SKCommandBuffer cb, void *dataGpu, u32 grid_dimensions[3]);
typedef void (*PFN_sk_dispatch_indirect) (SKCommandBuffer cb, void *dataGpu, void *grid_dimensions_gpu);

typedef void (*PFN_sk_begin_render_pass) (SKCommandBuffer cb, SKRenderPassDesc *desc);
typedef void (*PFN_sk_end_render_pass)   (SKCommandBuffer cb);

typedef void (*PFN_sk_draw_indexed_instanced)                (SKCommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count);
typedef void (*PFN_sk_draw_indexed_instanced_indirect)       (SKCommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, void *args_gpu);
typedef void (*PFN_sk_draw_indexed_instanced_indirect_multi) (SKCommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *args_gpu, void *draw_count_gpu, u32 stride);

typedef void (*PFN_sk_draw_meshlets)          (SKCommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, u32 dim[3]);
typedef void (*PFN_sk_draw_meshlets_indirect) (SKCommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, void *dim_gpu);

// allocation_callbacks can be nullptr, in which case the SK will use a default allocator that uses malloc/free. 
// If allocation_callbacks is not nullptr, the SK will use the provided allocator for all memory allocations and deallocations. 
// The user_data field of the allocation callbacks can be used to pass additional information to the allocator functions, such as a pointer to a custom allocator object or a logging function. 
// The SK will not modify the allocation callbacks or the user_data pointer, so it is the responsibility of the caller to ensure that they remain valid for the lifetime of the SK.
// The allocation_callbacks cannot be changed after initialization.

// Loader
void sk_load();
void sk_unload();

// Setup/Teardown
extern PFN_sk_init     sk_init;
extern PFN_sk_shutdown sk_shutdown;

// Memory
extern PFN_sk_alloc              sk_alloc;
extern PFN_sk_free               sk_free;
extern PFN_sk_host_to_device_ptr sk_host_to_device_ptr;

// Device
extern PFN_sk_create_device    sk_create_device;
extern PFN_sk_destroy_device   sk_destroy_device;
extern PFN_sk_device_wait_idle sk_device_wait_idle;

// Swapchain
extern PFN_sk_create_swapchain  sk_create_swapchain;
extern PFN_sk_destroy_swapchain sk_destroy_swapchain;
extern PFN_sk_next_backbuffer   sk_next_backbuffer;
extern PFN_sk_present           sk_present;

// Textures
extern PFN_sk_texture_size_align         sk_texture_size_align;
extern PFN_sk_create_texture             sk_create_texture;
extern PFN_sk_destroy_texture            sk_destroy_texture;
extern PFN_sk_texture_view_descriptor    sk_texture_view_descriptor;
extern PFN_sk_rw_texture_view_descriptor sk_rw_texture_view_descriptor;

// Pipelines
extern PFN_sk_create_compute_pipeline          sk_create_compute_pipeline;
extern PFN_sk_create_graphics_pipeline         sk_create_graphics_pipeline;
extern PFN_sk_create_graphics_meshlet_pipeline sk_create_graphics_meshlet_pipeline;
extern PFN_sk_destroy_pipeline                 sk_destroy_pipeline;

// State objects
extern PFN_sk_create_depth_stencil_state sk_create_depth_stencil_state;
extern PFN_sk_create_blend_state         sk_create_blend_state;
extern PFN_sk_free_depth_stencil_state   sk_free_depth_stencil_state;
extern PFN_sk_free_blend_state           sk_free_blend_state;

// Queue
extern PFN_sk_get_queue               sk_get_queue;
extern PFN_sk_start_command_recording sk_start_command_recording;
extern PFN_sk_submit                  sk_submit;

// Semaphores
extern PFN_sk_create_semaphore  sk_create_semaphore;
extern PFN_sk_wait_semaphore    sk_wait_semaphore;
extern PFN_sk_destroy_semaphore sk_destroy_semaphore;

// Commands
extern PFN_sk_mem_copy          sk_mem_copy;
extern PFN_sk_copy_to_texture   sk_copy_to_texture;
extern PFN_sk_copy_from_texture sk_copy_from_texture;

extern PFN_sk_set_active_texture_heap_ptr sk_set_active_texture_heap_ptr;

extern PFN_sk_barrier      sk_barrier;
extern PFN_sk_signal_after sk_signal_after;
extern PFN_sk_wait_before  sk_wait_before;

extern PFN_sk_signal_after2 sk_signal_after2;
extern PFN_sk_wait_before2  sk_wait_before2;

extern PFN_sk_set_pipeline            sk_set_pipeline;
extern PFN_sk_set_depth_stencil_state sk_set_depth_stencil_state;
extern PFN_sk_set_blend_state         sk_set_blend_state;

extern PFN_sk_dispatch          sk_dispatch;
extern PFN_sk_dispatch_indirect sk_dispatch_indirect;

extern PFN_sk_begin_render_pass sk_begin_render_pass;
extern PFN_sk_end_render_pass   sk_end_render_pass;

extern PFN_sk_draw_indexed_instanced                sk_draw_indexed_instanced;
extern PFN_sk_draw_indexed_instanced_indirect       sk_draw_indexed_instanced_indirect;
extern PFN_sk_draw_indexed_instanced_indirect_multi sk_draw_indexed_instanced_indirect_multi;

extern PFN_sk_draw_meshlets          sk_draw_meshlets;
extern PFN_sk_draw_meshlets_indirect sk_draw_meshlets_indirect;

#ifndef __cplusplus
}
#endif

#endif // SK_H