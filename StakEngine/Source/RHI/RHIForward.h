#pragma once

#include "Types.h"

class Rhi_Viewable_Resource;
using Rhi_Viewable_Resource_Ref = Ref<Rhi_Viewable_Resource>;
class Rhi_Pixel_Shader;
using Rhi_Pixel_Shader_Ref = Ref<Rhi_Pixel_Shader>;
class Rhi_Vertex_Shader;
using Rhi_Vertex_Shader_Ref = Ref<Rhi_Vertex_Shader>;
class Rhi_Geometry_Shader;
using Rhi_Geometry_Shader_Ref = Ref<Rhi_Geometry_Shader>;
class Rhi_Buffer;
using Rhi_Buffer_Ref = Ref<Rhi_Buffer>;
class Rhu_Uniform_Buffer;
using Rhi_Uniform_Buffer_Ref = Ref<Rhu_Uniform_Buffer>;
class Rhi_Staging_Buffer;
using Rhi_Staging_Buffer_Ref = Ref<Rhi_Staging_Buffer>;
class Rhi_Texture;
using Rhi_Texture_Ref = Ref<Rhi_Texture>;
class Rhi_Graphics_Pipeline_State;
using Rhi_Graphics_Pipeline_State_Ref = Ref<Rhi_Graphics_Pipeline_State>;
class Rhi_Sampler_State;
using Rhi_Sampler_State_Ref = Ref<Rhi_Sampler_State>;
class Rhi_Rasterizer_State;
using Rhi_Rasterizer_State_ref = Ref<Rhi_Rasterizer_State>;
class Rhi_Depth_Stencil_State;
using Rhi_Depth_Stencil_State_Ref = Ref<Rhi_Depth_Stencil_State>;
class Rhi_Blend_State;
using Rhi_Blend_State_Ref = Ref<Rhi_Blend_State>;
class Rhi_Vertex_Declaration;
using Rhi_Vertex_Declaration_Ref = Ref<Rhi_Vertex_Declaration>;
class Rhi_Shader_Resource_View;
using Rhi_Shader_Resource_View_Ref = Ref<Rhi_Shader_Resource_View>;
class Rhi_Unordered_Access_View;
using Rhi_Unordered_Access_View_Ref = Ref<Rhi_Unordered_Access_View>;
class Rhi_Command_List;
class Rhi_Command_Context;
class Rhi_Viewport;
using Rhi_Viewport_Ref = Ref<Rhi_Viewport>;
class Rhi_Transition;