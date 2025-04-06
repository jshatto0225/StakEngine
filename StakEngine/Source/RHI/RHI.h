#pragma once

#include "Types.h"

#include <imgui.h>

typedef Handle Rhi_Resource_Handle;

enum class Rhi_Backend {
    VULKAN,
};

enum class Rhi_Resource_State {
    UNDEFINED,
    RENDER_TARGET,
    SHADER_RESOURCE,
    PRESENT,
};

enum class Rhi_Transition_Type {
    INVALID,
    IMAGE,
    BUFFER,
};

struct Rhi_Transition_Barrier {
    Rhi_Resource_Handle resource;
    Rhi_Resource_State state_before;
    Rhi_Resource_State state_after;
    u32 subresource;
};

struct Rhi_Uav_Barrier {
    Rhi_Resource_Handle resource;
};

struct Rhi_Aliasing_Barrier {
    Rhi_Resource_Handle resource_before;
    Rhi_Resource_Handle resource_after;
};

enum class Rhi_Barrier_Type {
    TRANSITION,
    //UAV,
    //ALIASING
};

enum class Rhi_Format {
    UNDEFINED,
    B8G8R8A8_SRGB,
};

struct Rhi_Resource_Barrier {
    Rhi_Barrier_Type type;

    union {
        Rhi_Transition_Barrier transition_barrier;
        Rhi_Uav_Barrier uav_barrier;
        Rhi_Aliasing_Barrier aliasing_barrier;
    };
};

enum class Rhi_Resource_Type {
    TEXTURE,
    BUFFER,
};

struct Rhi_Render_Area {
    u32 layer_count;
    u32 x;
    u32 y;
    u32 width;
    u32 height;
};

struct Rhi_Texture_Description {
    u32 width;
    u32 height;
    Rhi_Format format;
    bool use_for_imgui;
    bool use_as_shader_resource;
};

enum class Rhi_Shader_Type {
    VERTEX,
    FRAGMENT
};

struct Rhi_Shader_Description {
    Rhi_Shader_Type type;
    const char *name;
};

struct Rhi_Buffer_Element {
    u32 location;
    u32 binding;
    Rhi_Format format;
    u32 offset;
};

struct Rhi_Buffer_Layout {
    u32 element_count;
    Rhi_Buffer_Element *elements;
    u32 stride;
};

enum class Rhi_Buffer_Type {
    VERTEX,
    INDEX
};

struct Rhi_Vertex_Buffer_Description {
    Rhi_Buffer_Layout layout;
    u32 element_count;
    void *initial_contents; // NOTE: Optional
    u32 initial_contents_size; // NOTE: Optional
    bool use_staging_buffer;
};

struct Rhi_Index_Buffer_Description {
    u32 count;
    void *initial_contents;
    u32 initial_contents_size;
    bool use_staging_buffer;
};

struct Rhi_Pipeline_Layout_Description {
    u32 descriptor_set_layout_count;
    Rhi_Resource_Handle *descriptor_set_layouts;
};

enum class Rhi_Pipeline_Type {
    GRAPHICS
};

enum class Rhi_Vertex_Input_Rate {
    PER_VERTEX,
    PER_INSTANCE,
};

struct Rhi_Vertex_Input_Binding {
    u32 binding;
    u32 stride;
    Rhi_Vertex_Input_Rate input_rate;
};

struct Rhi_Vertex_Input_Attribute {
    u32 location;
    u32 binding;
    Rhi_Format format;
    u32 offset;
};

struct Rhi_Graphics_Pipeline_State_Description {
    u32 color_format_count;
    Rhi_Format *color_formats;
    Rhi_Format depth_stencil_format;

    u32 shader_count;
    Rhi_Resource_Handle *shaders;

    u32 vertex_input_binding_count;
    Rhi_Vertex_Input_Binding *vertex_input_bindings;
    u32 vertex_input_attribute_count;
    Rhi_Vertex_Input_Attribute *vertex_input_attributes;

    Rhi_Resource_Handle layout;
};

enum class Rhi_Descriptor_Type {
    UNIFORM_BUFFER,
    TEXTURE
};

struct Rhi_Descriptor {
    u32 binding;
    Rhi_Descriptor_Type type;
    u32 count;
    u32 shader_stage_count;
    Rhi_Shader_Type *stages;
};

struct Rhi_Descriptor_Set_Layout_Description {
    u32 descriptor_count;
    Rhi_Descriptor *descriptors;
};

struct Rhi_Command_List_Description {
    bool is_secondary;
};

struct Window;

struct Rhi {
    bool (*init)();
    void (*shutdown)();

    void (*imgui_new_frame)();
    void (*shutdown_imgui)();
    void (*init_imgui)(Rhi_Resource_Handle);

    bool (*wait_for_gpu_idle)();

    bool (*prepare_frame)(Rhi_Resource_Handle);
    bool (*present_frame)();
    bool (*submit)(Rhi_Resource_Handle);

    void (*add_texture_to_imgui_window)(Rhi_Resource_Handle);

    Rhi_Resource_Handle (*create_texture)(Rhi_Texture_Description *);
    void (*destroy_texture)(Rhi_Resource_Handle *);

    Rhi_Resource_Handle (*create_swapchain)(Window *);
    void (*destroy_swapchain)(Rhi_Resource_Handle *);

    Rhi_Resource_Handle(*create_command_list)(Rhi_Command_List_Description *);
    void (*destroy_command_list)(Rhi_Resource_Handle *);

    Rhi_Resource_Handle (*create_shader)(Rhi_Shader_Description *);
    void (*destroy_shader)(Rhi_Resource_Handle *);

    Rhi_Resource_Handle(*create_vertex_buffer)(Rhi_Vertex_Buffer_Description *);
    Rhi_Resource_Handle(*create_index_buffer)(Rhi_Index_Buffer_Description *);
    void (*destroy_buffer)(Rhi_Resource_Handle *);

    Rhi_Resource_Handle (*create_pipeline_layout)(Rhi_Pipeline_Layout_Description *);
    void (*destroy_pipeline_layout)(Rhi_Resource_Handle *);

    Rhi_Resource_Handle (*create_descriptor_set_layout)(Rhi_Descriptor_Set_Layout_Description *);
    void (*destroy_descriptor_set_layout)(Rhi_Resource_Handle *);

    Rhi_Resource_Handle (*create_graphics_pipeline)(Rhi_Graphics_Pipeline_State_Description *);
    void (*destroy_pipeline)(Rhi_Resource_Handle *);

    void (*set_buffer_data)(Rhi_Resource_Handle, void *, u32);
    void (*map_buffer_data)(Rhi_Resource_Handle, void **, u32);
    bool (*send_buffer_to_gpu)(Rhi_Resource_Handle);

    Rhi_Resource_Handle (*get_current_swapchain_texture)(Rhi_Resource_Handle);
    Rhi_Format (*get_texture_format)(Rhi_Resource_Handle);
    Rhi_Render_Area (*get_texture_render_area)(Rhi_Resource_Handle);

    bool (*begin_command_list)(Rhi_Resource_Handle);
    bool (*end_command_list)(Rhi_Resource_Handle);

    void (*cmd_render_imgui_draw_data)(Rhi_Resource_Handle, ImDrawData *);
    void (*cmd_resource_barrier)(Rhi_Resource_Handle, Rhi_Resource_Barrier *);
    void (*cmd_set_render_target)(Rhi_Resource_Handle, Rhi_Resource_Handle, Rhi_Render_Area *);
    void (*cmd_unset_render_target)(Rhi_Resource_Handle);
    void (*cmd_bind_vertex_buffer)(Rhi_Resource_Handle, Rhi_Resource_Handle, u32);
    void (*cmd_bind_index_buffer)(Rhi_Resource_Handle, Rhi_Resource_Handle);
    void (*cmd_draw_indexed)(Rhi_Resource_Handle, u32, u32, u32, s32, u32);
    void (*cmd_draw_instanced)(Rhi_Resource_Handle, u32, u32, u32, u32);
    void (*cmd_set_viewport)(Rhi_Resource_Handle, f32, f32, f32, f32, f32, f32);
    void (*cmd_set_scissor)(Rhi_Resource_Handle, s32, s32, u32, u32);
    void (*cmd_bind_pipeline)(Rhi_Resource_Handle, Rhi_Resource_Handle);

    void (*notify_swapchain_of_resize)(Rhi_Resource_Handle);
};

Rhi_Transition_Type rhi_get_transition_type(Rhi_Resource_State before, Rhi_Resource_State after);

bool rhi_init(Rhi_Backend backend);
void rhi_shutdown();

extern Rhi rhi;
