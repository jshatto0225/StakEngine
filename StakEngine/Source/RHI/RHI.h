#pragma once

#include "Types.h"
#include "RHIForward.h"

#include <imgui.h>

enum class Rhi_Access {

};

enum class Rhi_Uniform_Buffer_Usage {

};

enum class Rhi_Buffer_Usage_Flags {

};

enum class Rhi_Transition_Create_Flags {

};

struct Rhi_Uniform_Buffer_Layout {

};

struct Rhi_Buffer_Description {

};

struct Rhi_Sampler_State_Initializer {

};

struct Rhi_Rasterizer_State_Initializer {

};

struct Rhi_Depth_Stencil_State_Initializer {

};

struct Rhi_Blend_State_Initializer {

};

struct Rhi_Vertex_Declaration_Element_List {

};

struct Rhi_Graphics_Pipeline_State_Initializer {

};

struct Rhi_Texture_Description {

};

struct Rhi_View_Description {

};

struct Rhi_Resource_Create_Info {

};

class Rhi {
public:
    Rhi();
    virtual ~Rhi();
    
    Rhi(Rhi &) = delete;
    Rhi &operator=(Rhi &) = delete;
    
public:
    inline static Rhi &get() { return *instance; }
    
    static void init();
    static void shutdown();
    
    virtual Rhi_Sampler_State_Ref create_sampler_state(const Rhi_Sampler_State_Initializer &initializer) = 0;
    virtual Rhi_Rasterizer_State_ref create_rasterizer_state(const Rhi_Rasterizer_State_Initializer &initializer) = 0;
    virtual Rhi_Depth_Stencil_State_Ref create_depth_stencil_state(const Rhi_Depth_Stencil_State_Initializer &initializer) = 0;
    virtual Rhi_Blend_State_Ref create_blend_state(const Rhi_Blend_State_Initializer &initializer) = 0;
    virtual Rhi_Vertex_Declaration_Ref create_vertex_declaration(const Rhi_Vertex_Declaration_Element_List &elements) = 0;
    
    virtual Rhi_Pixel_Shader_Ref create_pixel_shader() = 0;
    virtual Rhi_Vertex_Shader_Ref create_vertex_shader() = 0;
    virtual Rhi_Geometry_Shader_Ref create_geometry_shader() = 0;
    
    virtual Rhi_Buffer_Ref create_buffer(const Rhi_Buffer_Description &description, Rhi_Access access) = 0;
    virtual Rhi_Uniform_Buffer_Ref create_uniform_buffer(const void *initialContents, const Rhi_Uniform_Buffer_Layout &layout, Rhi_Uniform_Buffer_Usage usage) = 0;
    virtual Rhi_Staging_Buffer_Ref create_staging_buffer() = 0;
    
    virtual Rhi_Texture_Ref create_texture(const Rhi_Texture_Description &description) = 0;
    
    virtual Rhi_Graphics_Pipeline_State_Ref create_graphics_pipeline_state(const Rhi_Graphics_Pipeline_State_Initializer &initializer) = 0;
    
    virtual Rhi_Shader_Resource_View_Ref create_shader_resource_view(Rhi_Viewable_Resource_Ref resource, const Rhi_View_Description &description) = 0;
    virtual Rhi_Unordered_Access_View_Ref create_unordered_access_view(Rhi_Viewable_Resource_Ref resource, const Rhi_View_Description &description) = 0;
    
    virtual void submit(Rhi_Command_List &command_list) final;
    
    virtual void init_imgui() = 0;
    virtual void shutdown_imgui() = 0;
    virtual void imgui_new_frame() = 0;
    virtual void submit_imgui_draw_data(ImDrawData *draw_data) = 0;
    
    virtual void begin_drawing() = 0;
    virtual void end_drawing() = 0;
    virtual void is_framebuffer_resized() = 0;
    
    virtual void wait_for_gpu_idle() = 0;
    
    virtual Rhi_Command_Context *get_command_context() = 0;
    
private:
    inline static Rhi *instance;
};
