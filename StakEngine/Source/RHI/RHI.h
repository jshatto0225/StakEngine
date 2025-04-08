#pragma once

#include "Types.h"

#include <imgui.h>

typedef Handle RHIResourceHandle;

enum class RHIBackend {
    Vulkan,
};

enum class RHIResourceState {
    Undefined,
    RenderTarget,
    ShaderResource,
    Present,
};

enum class RHITransitionType {
    Invalid,
    Image,
    Buffer,
};

struct RHITransitionBarrier {
    RHIResourceHandle resource;
    RHIResourceState state_before;
    RHIResourceState state_after;
    u32 subresource;
};

struct RHIUAVBarrier {
    RHIResourceHandle resource;
};

struct RHIAliasingBarrier {
    RHIResourceHandle resource_before;
    RHIResourceHandle resource_after;
};

enum class RHIBarrierType {
    Transition,
    //UAV,
    //ALIASING
};

enum class RHIFormat {
    Undefined,
    B8G8R8A8Srgb,
};

struct Rhi_Resource_Barrier {
    RHIBarrierType type;

    union {
        RHITransitionBarrier transition_barrier;
        RHIUAVBarrier uav_barrier;
        RHIAliasingBarrier aliasing_barrier;
    };
};

enum class RHIResourceType {
    Texture,
    Buffer,
};

struct RHIRenderArea {
    u32 layer_count;
    u32 x;
    u32 y;
    u32 width;
    u32 height;
};

struct RHITextureDescription {
    u32 width;
    u32 height;
    RHIFormat format;
    bool use_for_imgui;
    bool use_as_shader_resource;
};

enum class RHIShaderType {
    Vertex,
    Fragment
};

struct RHIShaderDescription {
    RHIShaderType type;
    const char *name;
};

struct RHIBufferElement {
    u32 location;
    u32 binding;
    RHIFormat format;
    u32 offset;
};

struct RHIBufferLayout {
    u32 element_count;
    RHIBufferElement *elements;
    u32 stride;
};

enum class RHIBufferType {
    Vertex,
    Index
};

struct RHIVertexBufferDescription {
    RHIBufferLayout layout;
    u32 element_count;
    void *initial_contents; // NOTE: Optional
    u32 initial_contents_size; // NOTE: Optional
    bool use_staging_buffer;
};

struct RHIIndexBufferDescription {
    u32 count;
    void *initial_contents;
    u32 initial_contents_size;
    bool use_staging_buffer;
};

struct RHIPipelineLayoutDescription {
    u32 descriptor_set_layout_count;
    RHIResourceHandle *descriptor_set_layouts;
};

enum class RHIPipelineType {
    Graphics
};

enum class RHIVertexInputRate {
    Vertex,
    Instance,
};

struct RHIVertexInputBinding {
    u32 binding;
    u32 stride;
    RHIVertexInputRate input_rate;
};

struct RHIVertexInputAttribute {
    u32 location;
    u32 binding;
    RHIFormat format;
    u32 offset;
};

struct RHIGraphicsPipelineStateDescription {
    u32 color_format_count;
    RHIFormat *color_formats;
    RHIFormat depth_stencil_format;

    u32 shader_count;
    RHIResourceHandle *shaders;

    u32 vertex_input_binding_count;
    RHIVertexInputBinding *vertex_input_bindings;
    u32 vertex_input_attribute_count;
    RHIVertexInputAttribute *vertex_input_attributes;

    RHIResourceHandle layout;
};

enum class RHIDescriptorType {
    UniformBuffer,
    Texture
};

struct RHIDescriptor {
    u32 binding;
    RHIDescriptorType type;
    u32 count;
    u32 shader_stage_count;
    RHIShaderType *stages;
};

struct RHIDescriptorSetLayoutDescription {
    u32 descriptor_count;
    RHIDescriptor *descriptors;
};

struct RHICommandListDescription {
    bool is_secondary;
};

struct Window;

struct RHI {
    bool (*init)();
    void (*shutdown)();

    void (*imgui_new_frame)();
    void (*shutdown_imgui)();
    void (*init_imgui)(RHIResourceHandle);

    bool (*wait_for_gpu_idle)();

    bool (*prepare_frame)(RHIResourceHandle);
    bool (*present_frame)();
    bool (*submit)(RHIResourceHandle);

    void (*add_texture_to_imgui_window)(RHIResourceHandle);

    RHIResourceHandle (*create_texture)(RHITextureDescription *);
    void (*destroy_texture)(RHIResourceHandle *);

    RHIResourceHandle (*create_swapchain)(Window *);
    void (*destroy_swapchain)(RHIResourceHandle *);

    RHIResourceHandle(*create_command_list)(RHICommandListDescription *);
    void (*destroy_command_list)(RHIResourceHandle *);

    RHIResourceHandle (*create_shader)(RHIShaderDescription *);
    void (*destroy_shader)(RHIResourceHandle *);

    RHIResourceHandle(*create_vertex_buffer)(RHIVertexBufferDescription *);
    RHIResourceHandle(*create_index_buffer)(RHIIndexBufferDescription *);
    void (*destroy_buffer)(RHIResourceHandle *);

    RHIResourceHandle (*create_pipeline_layout)(RHIPipelineLayoutDescription *);
    void (*destroy_pipeline_layout)(RHIResourceHandle *);

    RHIResourceHandle (*create_descriptor_set_layout)(RHIDescriptorSetLayoutDescription *);
    void (*destroy_descriptor_set_layout)(RHIResourceHandle *);

    RHIResourceHandle (*create_graphics_pipeline)(RHIGraphicsPipelineStateDescription *);
    void (*destroy_pipeline)(RHIResourceHandle *);

    void (*set_buffer_data)(RHIResourceHandle, void *, u32);
    void (*map_buffer_data)(RHIResourceHandle, void **, u32);
    bool (*send_buffer_to_gpu)(RHIResourceHandle);

    RHIResourceHandle (*get_current_swapchain_texture)(RHIResourceHandle);
    RHIFormat (*get_texture_format)(RHIResourceHandle);
    RHIRenderArea (*get_texture_render_area)(RHIResourceHandle);

    bool (*begin_command_list)(RHIResourceHandle);
    bool (*end_command_list)(RHIResourceHandle);

    void (*cmd_render_imgui_draw_data)(RHIResourceHandle, ImDrawData *);
    void (*cmd_resource_barrier)(RHIResourceHandle, Rhi_Resource_Barrier *);
    void (*cmd_set_render_target)(RHIResourceHandle, RHIResourceHandle, RHIRenderArea *);
    void (*cmd_unset_render_target)(RHIResourceHandle);
    void (*cmd_bind_vertex_buffer)(RHIResourceHandle, RHIResourceHandle, u32);
    void (*cmd_bind_index_buffer)(RHIResourceHandle, RHIResourceHandle);
    void (*cmd_draw_indexed)(RHIResourceHandle, u32, u32, u32, s32, u32);
    void (*cmd_draw_instanced)(RHIResourceHandle, u32, u32, u32, u32);
    void (*cmd_set_viewport)(RHIResourceHandle, f32, f32, f32, f32, f32, f32);
    void (*cmd_set_scissor)(RHIResourceHandle, s32, s32, u32, u32);
    void (*cmd_bind_pipeline)(RHIResourceHandle, RHIResourceHandle);

    void (*notify_swapchain_of_resize)(RHIResourceHandle);
};

RHITransitionType rhi_get_transition_type(RHIResourceState before, RHIResourceState after);

bool rhi_init(RHIBackend backend);
void rhi_shutdown();

extern RHI rhi;
