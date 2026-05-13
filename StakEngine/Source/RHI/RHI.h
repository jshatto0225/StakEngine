#pragma once

#include "Types.h"

#include <imgui.h>

// Opaque handles
struct RHIPipeline {
    u64 data;
};

struct RHITexture {
    u64 data;
};

struct RHIDepthStencilState {
    u64 data;
};

struct RHIBlendState {
    u64 data;
};

struct RHIQueue {
    u64 data;
};

struct RHICommandBuffer {
    u64 data;
};

struct RHISemaphore {
    u64 data;
};

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
    RHI_FORMAT_D32_FLOAT,
    RHI_FORMAT_RG11B10_FLOAT,
    RHI_FORMAT_RGB10_A2_UNORM 
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

struct RHI {
    // Memory
    void *(*alloc)(u64 bytes, RHIMemoryType memory);
    void (*free)(void *ptr);
    void *(*host_to_device_pointer)(void *ptr);

    // Textures
    RHITextureSizeAlign (*texture_size_align)(RHITextureDesc desc);
    RHITexture (*create_texture)(RHITextureDesc desc, void *ptr_gpu);
    RHITextureDescriptor (*texture_view_descriptor)(RHITexture texture, RHIViewDesc desc);
    RHITextureDescriptor (*rw_texture_view_descriptor)(RHITexture texture, RHIViewDesc desc);

    // Pipelines
    RHIPipeline (*create_compute_pipeline)(u8 *compute_ir, u32 ir_size);
    RHIPipeline (*create_graphics_pipeline)(u8 *vertex_ir, u32 vertex_ir_size, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc desc);
    RHIPipeline (*create_graphics_meshlet_pipeline)(u8 meshlet_ir, u32 meshlet_ir_length, u8 *pixel_ir, u32 pixel_ir_size, RHIRasterDesc desc);
    void (*free_pipeline)(RHIPipeline pipeline);

    // State objects
    RHIDepthStencilState (*create_depth_stencil_state)(RHIDepthStencilDesc desc);
    RHIBlendState (*create_blend_state)(RHIBlendDesc desc);
    void (*free_depth_stencil_state)(RHIDepthStencilState state);
    void (*free_blend_state)(RHIBlendState state);

    // Queue
    RHIQueue (*create_queue)(/* DEVICE & QUEUE CREATION DETAILS OMITTED */);
    RHICommandBuffer (*start_command_recording)(RHIQueue queue);
    void (*submit)(RHIQueue queue, RHICommandBuffer *command_buffers, u32 command_buffer_count, RHISemaphore semaphore);

    // Semaphores
    RHISemaphore (*create_semaphore)(u64 init_value);
    void (*wait_semaphore)(RHISemaphore semaphore, u64 value);
    void (*destroy_semaphore)(RHISemaphore semaphore);

    // Commands
    void (*mem_copy)(RHICommandBuffer cb, void *dest_gpu, void *src_gpu, u64 size);
    void (*copy_to_texture)(RHICommandBuffer cb, RHITexture texture, void *src_gpu);
    void (*copy_from_texture)(RHICommandBuffer cb, void *dest_gpu, RHITexture texture);

    void (*set_active_texture_heap_ptr)(RHICommandBuffer cb, void *ptr_gpu);
    void (*set_active_resource_heap_ptr)(RHICommandBuffer cb, void *ptr_gpu);

    void (*barrier)(RHICommandBuffer cb, RHIPipelineStage before, RHIPipelineStage after, RHIHazardFlags hazards);
    void (*signal_after)(RHICommandBuffer cb, RHIPipelineStage before, void *ptr_gpu, u64 value, RHISignal signal);
    void (*wait_before)(RHICommandBuffer cb, RHIPipelineStage after, void *ptr_gpu, u64 value, RHIOp op, RHIHazardFlags hazards, u64 mask);

    void (*set_pipeline)(RHICommandBuffer cb, RHIPipeline pipeline);
    void (*set_depth_stencil_state)(RHICommandBuffer cb, RHIDepthStencilState state);
    void (*set_blend_state)(RHICommandBuffer cb, RHIBlendState state);

    void (*dispatch)(RHICommandBuffer cb, void *dataGpu, u32 grid_dimensions[3]);
    void (*dispatch_indirect)(RHICommandBuffer cb, void *dataGpu, void *grid_dimensions_gpu);

    void (*begin_render_pass)(RHICommandBuffer cb, RHIRenderPassDesc desc);
    void (*end_render_pass)(RHICommandBuffer cb);

    void (*draw_indexed_instanced)(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, u32 index_count, u32 instance_count);
    void (*draw_indexed_instanced_indirect)(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *indices_gpu, void *args_gpu);
    void (*draw_indexed_instanced_indirect_multi)(RHICommandBuffer cb, void *vertex_data_gpu, void *pixel_data_gpu, void *args_gpu, void *draw_count_gpu, u32 stride);

    void (*draw_meshlets)(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, u32 dim[3]);
    void (*draw_meshlets_indirect)(RHICommandBuffer cb, void *meshlet_data_gpu, void *pixel_data_gpu, void *dim_gpu);
};

bool rhi_init();
void rhi_shutdown();

extern RHI rhi;