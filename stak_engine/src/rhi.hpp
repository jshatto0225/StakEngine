#pragma once

#include "utils.hpp"
#include "window.hpp"

#include <imgui.h>

struct Rhi_Device;
struct Rhi_Render_Pass;
struct Rhi_Subpass;
struct Rhi_Pipeline;
struct Rhi_Index_Buffer;
struct Rhi_Vertex_Buffer;
struct Rhi_Uniform_Buffer;
struct Rhi_Command_List;
struct Rhi_Shader;
struct Rhi_Texture;
struct Rhi_Recipt;
struct Rhi_Sync_Object;
struct Rhi_Render_Pass;
struct Rhi_Render_Target;

// NOTE: The enums match the values in teh vulkan 1.0 spec

enum class Rhi_Vendor : u8 {
	NONE,
	VULKAN,
    DIRECTX,
    METAL,
};

enum class Rhi_Sync_Type {
    FENCE,
    SEMAPHORE, // NOTE: This only works in vulkan
};

enum Rhi_Pipeline_Stage_Flags {
    RHI_PIPELINE_STAGE_TOP_OF_PIPE                    = 0x00001,
    RHI_PIPELINE_STAGE_DRAW_INDIRECT                  = 0x00002,
    RHI_PIPELINE_STAGE_VERTEX_INPUT                   = 0x00004,
    RHI_PIPELINE_STAGE_VERTEX_SHADER                  = 0x00008,
    RHI_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER    = 0x00010,
    RHI_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00020,
    RHI_PIPELINE_STAGE_GEOMETRY_SHADER                = 0x00040,
    RHI_PIPELINE_STAGE_FRAGMENT_SHADER                = 0x00080,
    RHI_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS           = 0x00100,
    RHI_PIPELINE_STAGE_LATE_FRAGMENT_TESTS            = 0x00200,
    RHI_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT        = 0x00400,
    RHI_PIPELINE_STAGE_COMPUTE_SHADER                 = 0x00800,
    RHI_PIPELINE_STAGE_TRANSFER                       = 0x01000,
    RHI_PIPELINE_STAGE_BOTTOM_OF_PIPE                 = 0x02000,
    RHI_PIPELINE_STAGE_HOST                           = 0x04000,
    RHI_PIPELINE_STAGE_ALL_GRAPHICS                   = 0x08000,
    RHI_PIPELINE_STAGE_ALL_COMMANDS                   = 0x10000,
};

enum Rhi_Format {
    RHI_FORMAT_UNDEFINED = 0,
    RHI_FORMAT_R4G4_UNORM_PACK8 = 1,
    RHI_FORMAT_R4G4B4A4_UNORM_PACK16 = 2,
    RHI_FORMAT_B4G4R4A4_UNORM_PACK16 = 3,
    RHI_FORMAT_R5G6B5_UNORM_PACK16 = 4,
    RHI_FORMAT_B5G6R5_UNORM_PACK16 = 5,
    RHI_FORMAT_R5G5B5A1_UNORM_PACK16 = 6,
    RHI_FORMAT_B5G5R5A1_UNORM_PACK16 = 7,
    RHI_FORMAT_A1R5G5B5_UNORM_PACK16 = 8,
    RHI_FORMAT_R8_UNORM = 9,
    RHI_FORMAT_R8_SNORM = 10,
    RHI_FORMAT_R8_USCALED = 11,
    RHI_FORMAT_R8_SSCALED = 12,
    RHI_FORMAT_R8_UINT = 13,
    RHI_FORMAT_R8_SINT = 14,
    RHI_FORMAT_R8_SRGB = 15,
    RHI_FORMAT_R8G8_UNORM = 16,
    RHI_FORMAT_R8G8_SNORM = 17,
    RHI_FORMAT_R8G8_USCALED = 18,
    RHI_FORMAT_R8G8_SSCALED = 19,
    RHI_FORMAT_R8G8_UINT = 20,
    RHI_FORMAT_R8G8_SINT = 21,
    RHI_FORMAT_R8G8_SRGB = 22,
    RHI_FORMAT_R8G8B8_UNORM = 23,
    RHI_FORMAT_R8G8B8_SNORM = 24,
    RHI_FORMAT_R8G8B8_USCALED = 25,
    RHI_FORMAT_R8G8B8_SSCALED = 26,
    RHI_FORMAT_R8G8B8_UINT = 27,
    RHI_FORMAT_R8G8B8_SINT = 28,
    RHI_FORMAT_R8G8B8_SRGB = 29,
    RHI_FORMAT_B8G8R8_UNORM = 30,
    RHI_FORMAT_B8G8R8_SNORM = 31,
    RHI_FORMAT_B8G8R8_USCALED = 32,
    RHI_FORMAT_B8G8R8_SSCALED = 33,
    RHI_FORMAT_B8G8R8_UINT = 34,
    RHI_FORMAT_B8G8R8_SINT = 35,
    RHI_FORMAT_B8G8R8_SRGB = 36,
    RHI_FORMAT_R8G8B8A8_UNORM = 37,
    RHI_FORMAT_R8G8B8A8_SNORM = 38,
    RHI_FORMAT_R8G8B8A8_USCALED = 39,
    RHI_FORMAT_R8G8B8A8_SSCALED = 40,
    RHI_FORMAT_R8G8B8A8_UINT = 41,
    RHI_FORMAT_R8G8B8A8_SINT = 42,
    RHI_FORMAT_R8G8B8A8_SRGB = 43,
    RHI_FORMAT_B8G8R8A8_UNORM = 44,
    RHI_FORMAT_B8G8R8A8_SNORM = 45,
    RHI_FORMAT_B8G8R8A8_USCALED = 46,
    RHI_FORMAT_B8G8R8A8_SSCALED = 47,
    RHI_FORMAT_B8G8R8A8_UINT = 48,
    RHI_FORMAT_B8G8R8A8_SINT = 49,
    RHI_FORMAT_B8G8R8A8_SRGB = 50,
    RHI_FORMAT_A8B8G8R8_UNORM_PACK32 = 51,
    RHI_FORMAT_A8B8G8R8_SNORM_PACK32 = 52,
    RHI_FORMAT_A8B8G8R8_USCALED_PACK32 = 53,
    RHI_FORMAT_A8B8G8R8_SSCALED_PACK32 = 54,
    RHI_FORMAT_A8B8G8R8_UINT_PACK32 = 55,
    RHI_FORMAT_A8B8G8R8_SINT_PACK32 = 56,
    RHI_FORMAT_A8B8G8R8_SRGB_PACK32 = 57,
    RHI_FORMAT_A2R10G10B10_UNORM_PACK32 = 58,
    RHI_FORMAT_A2R10G10B10_SNORM_PACK32 = 59,
    RHI_FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
    RHI_FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
    RHI_FORMAT_A2R10G10B10_UINT_PACK32 = 62,
    RHI_FORMAT_A2R10G10B10_SINT_PACK32 = 63,
    RHI_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
    RHI_FORMAT_A2B10G10R10_SNORM_PACK32 = 65,
    RHI_FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
    RHI_FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
    RHI_FORMAT_A2B10G10R10_UINT_PACK32 = 68,
    RHI_FORMAT_A2B10G10R10_SINT_PACK32 = 69,
    RHI_FORMAT_R16_UNORM = 70,
    RHI_FORMAT_R16_SNORM = 71,
    RHI_FORMAT_R16_USCALED = 72,
    RHI_FORMAT_R16_SSCALED = 73,
    RHI_FORMAT_R16_UINT = 74,
    RHI_FORMAT_R16_SINT = 75,
    RHI_FORMAT_R16_SFLOAT = 76,
    RHI_FORMAT_R16G16_UNORM = 77,
    RHI_FORMAT_R16G16_SNORM = 78,
    RHI_FORMAT_R16G16_USCALED = 79,
    RHI_FORMAT_R16G16_SSCALED = 80,
    RHI_FORMAT_R16G16_UINT = 81,
    RHI_FORMAT_R16G16_SINT = 82,
    RHI_FORMAT_R16G16_SFLOAT = 83,
    RHI_FORMAT_R16G16B16_UNORM = 84,
    RHI_FORMAT_R16G16B16_SNORM = 85,
    RHI_FORMAT_R16G16B16_USCALED = 86,
    RHI_FORMAT_R16G16B16_SSCALED = 87,
    RHI_FORMAT_R16G16B16_UINT = 88,
    RHI_FORMAT_R16G16B16_SINT = 89,
    RHI_FORMAT_R16G16B16_SFLOAT = 90,
    RHI_FORMAT_R16G16B16A16_UNORM = 91,
    RHI_FORMAT_R16G16B16A16_SNORM = 92,
    RHI_FORMAT_R16G16B16A16_USCALED = 93,
    RHI_FORMAT_R16G16B16A16_SSCALED = 94,
    RHI_FORMAT_R16G16B16A16_UINT = 95,
    RHI_FORMAT_R16G16B16A16_SINT = 96,
    RHI_FORMAT_R16G16B16A16_SFLOAT = 97,
    RHI_FORMAT_R32_UINT = 98,
    RHI_FORMAT_R32_SINT = 99,
    RHI_FORMAT_R32_SFLOAT = 100,
    RHI_FORMAT_R32G32_UINT = 101,
    RHI_FORMAT_R32G32_SINT = 102,
    RHI_FORMAT_R32G32_SFLOAT = 103,
    RHI_FORMAT_R32G32B32_UINT = 104,
    RHI_FORMAT_R32G32B32_SINT = 105,
    RHI_FORMAT_R32G32B32_SFLOAT = 106,
    RHI_FORMAT_R32G32B32A32_UINT = 107,
    RHI_FORMAT_R32G32B32A32_SINT = 108,
    RHI_FORMAT_R32G32B32A32_SFLOAT = 109,
    RHI_FORMAT_R64_UINT = 110,
    RHI_FORMAT_R64_SINT = 111,
    RHI_FORMAT_R64_SFLOAT = 112,
    RHI_FORMAT_R64G64_UINT = 113,
    RHI_FORMAT_R64G64_SINT = 114,
    RHI_FORMAT_R64G64_SFLOAT = 115,
    RHI_FORMAT_R64G64B64_UINT = 116,
    RHI_FORMAT_R64G64B64_SINT = 117,
    RHI_FORMAT_R64G64B64_SFLOAT = 118,
    RHI_FORMAT_R64G64B64A64_UINT = 119,
    RHI_FORMAT_R64G64B64A64_SINT = 120,
    RHI_FORMAT_R64G64B64A64_SFLOAT = 121,
    RHI_FORMAT_B10G11R11_UFLOAT_PACK32 = 122,
    RHI_FORMAT_E5B9G9R9_UFLOAT_PACK32 = 123,
    RHI_FORMAT_D16_UNORM = 124,
    RHI_FORMAT_X8_D24_UNORM_PACK32 = 125,
    RHI_FORMAT_D32_SFLOAT = 126,
    RHI_FORMAT_S8_UINT = 127,
    RHI_FORMAT_D16_UNORM_S8_UINT = 128,
    RHI_FORMAT_D24_UNORM_S8_UINT = 129,
    RHI_FORMAT_D32_SFLOAT_S8_UINT = 130,
    RHI_FORMAT_BC1_RGB_UNORM_BLOCK = 131,
    RHI_FORMAT_BC1_RGB_SRGB_BLOCK = 132,
    RHI_FORMAT_BC1_RGBA_UNORM_BLOCK = 133,
    RHI_FORMAT_BC1_RGBA_SRGB_BLOCK = 134,
    RHI_FORMAT_BC2_UNORM_BLOCK = 135,
    RHI_FORMAT_BC2_SRGB_BLOCK = 136,
    RHI_FORMAT_BC3_UNORM_BLOCK = 137,
    RHI_FORMAT_BC3_SRGB_BLOCK = 138,
    RHI_FORMAT_BC4_UNORM_BLOCK = 139,
    RHI_FORMAT_BC4_SNORM_BLOCK = 140,
    RHI_FORMAT_BC5_UNORM_BLOCK = 141,
    RHI_FORMAT_BC5_SNORM_BLOCK = 142,
    RHI_FORMAT_BC6H_UFLOAT_BLOCK = 143,
    RHI_FORMAT_BC6H_SFLOAT_BLOCK = 144,
    RHI_FORMAT_BC7_UNORM_BLOCK = 145,
    RHI_FORMAT_BC7_SRGB_BLOCK = 146,
    RHI_FORMAT_ETC2_R8G8B8_UNORM_BLOCK = 147,
    RHI_FORMAT_ETC2_R8G8B8_SRGB_BLOCK = 148,
    RHI_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK = 149,
    RHI_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK = 150,
    RHI_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK = 151,
    RHI_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK = 152,
    RHI_FORMAT_EAC_R11_UNORM_BLOCK = 153,
    RHI_FORMAT_EAC_R11_SNORM_BLOCK = 154,
    RHI_FORMAT_EAC_R11G11_UNORM_BLOCK = 155,
    RHI_FORMAT_EAC_R11G11_SNORM_BLOCK = 156,
    RHI_FORMAT_ASTC_4x4_UNORM_BLOCK = 157,
    RHI_FORMAT_ASTC_4x4_SRGB_BLOCK = 158,
    RHI_FORMAT_ASTC_5x4_UNORM_BLOCK = 159,
    RHI_FORMAT_ASTC_5x4_SRGB_BLOCK = 160,
    RHI_FORMAT_ASTC_5x5_UNORM_BLOCK = 161,
    RHI_FORMAT_ASTC_5x5_SRGB_BLOCK = 162,
    RHI_FORMAT_ASTC_6x5_UNORM_BLOCK = 163,
    RHI_FORMAT_ASTC_6x5_SRGB_BLOCK = 164,
    RHI_FORMAT_ASTC_6x6_UNORM_BLOCK = 165,
    RHI_FORMAT_ASTC_6x6_SRGB_BLOCK = 166,
    RHI_FORMAT_ASTC_8x5_UNORM_BLOCK = 167,
    RHI_FORMAT_ASTC_8x5_SRGB_BLOCK = 168,
    RHI_FORMAT_ASTC_8x6_UNORM_BLOCK = 169,
    RHI_FORMAT_ASTC_8x6_SRGB_BLOCK = 170,
    RHI_FORMAT_ASTC_8x8_UNORM_BLOCK = 171,
    RHI_FORMAT_ASTC_8x8_SRGB_BLOCK = 172,
    RHI_FORMAT_ASTC_10x5_UNORM_BLOCK = 173,
    RHI_FORMAT_ASTC_10x5_SRGB_BLOCK = 174,
    RHI_FORMAT_ASTC_10x6_UNORM_BLOCK = 175,
    RHI_FORMAT_ASTC_10x6_SRGB_BLOCK = 176,
    RHI_FORMAT_ASTC_10x8_UNORM_BLOCK = 177,
    RHI_FORMAT_ASTC_10x8_SRGB_BLOCK = 178,
    RHI_FORMAT_ASTC_10x10_UNORM_BLOCK = 179,
    RHI_FORMAT_ASTC_10x10_SRGB_BLOCK = 180,
    RHI_FORMAT_ASTC_12x10_UNORM_BLOCK = 181,
    RHI_FORMAT_ASTC_12x10_SRGB_BLOCK = 182,
    RHI_FORMAT_ASTC_12x12_UNORM_BLOCK = 183,
    RHI_FORMAT_ASTC_12x12_SRGB_BLOCK = 184,
};

enum Rhi_Sample_Count_Flags {
    RHI_SAMPLE_COUNT_1  = 0x01,
    RHI_SAMPLE_COUNT_2  = 0x02,
    RHI_SAMPLE_COUNT_4  = 0x04,
    RHI_SAMPLE_COUNT_8  = 0x08,
    RHI_SAMPLE_COUNT_16 = 0x10,
    RHI_SAMPLE_COUNT_32 = 0x20,
    RHI_SAMPLE_COUNT_64 = 0x40,
};

enum Rhi_Load_Op {
    RHI_LOAD_OP_LOAD = 0,
    RHI_LOAD_OP_CLEAR = 1,
    RHI_LOAD_OP_DONT_CARE = 2,
};

enum Rhi_Store_Op {
    RHI_STORE_OP_STORE = 0,
    RHI_STORE_OP_DONT_CARE = 1,
};

enum Rhi_Image_Layout {
    RHI_IMAGE_LAYOUT_UNDEFINED = 0,
    RHI_IMAGE_LAYOUT_GENERAL = 1,
    RHI_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
    RHI_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
    RHI_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
    RHI_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
    RHI_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
    RHI_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7,
    RHI_IMAGE_LAYOUT_PREINITIALIZED = 8,
    RHI_IMAGE_LAYOUT_PRESENT_SRC = 1000001002,
};

typedef u32 Rhi_Sample_Count_Mask;

struct Rhi_Attachment {
    bool may_alias = false;
    Rhi_Format format = RHI_FORMAT_UNDEFINED;
    Rhi_Sample_Count_Mask samples = 0;
    Rhi_Load_Op load_op = RHI_LOAD_OP_LOAD;
    Rhi_Store_Op store_op = RHI_STORE_OP_STORE;
    Rhi_Load_Op stencil_load_op = RHI_LOAD_OP_LOAD;
    Rhi_Store_Op stencil_store_op = RHI_STORE_OP_STORE;
    Rhi_Image_Layout initial_layout = RHI_IMAGE_LAYOUT_UNDEFINED;
    Rhi_Image_Layout final_layout = RHI_IMAGE_LAYOUT_UNDEFINED;
};

enum Rhi_Pipeline_Bind_Point {
    RHI_PIPELINE_BIND_POINT_GRAPHICS = 0,
    RHI_PIPELINE_BIND_POINT_COMPUTE = 1,
};

struct Rhi_Attachment_Ref {
    u32 attachment = 0;
    Rhi_Image_Layout layout = RHI_IMAGE_LAYOUT_UNDEFINED;
};

struct Rhi_Subpass {
    Rhi_Pipeline_Bind_Point pipeline_bind_point = RHI_PIPELINE_BIND_POINT_GRAPHICS;
    Array<Rhi_Attachment_Ref> input_attachments = {};
    Array<Rhi_Attachment_Ref> color_attachments = {};
    Array<Rhi_Attachment_Ref> resolve_attachments = {};
    Option<Rhi_Attachment_Ref> depth_stencil_attachment = {};
    Array<u32> preserve_attachments = {};
};

#define RHI_SUBPASS_EXTERNAL ~(0U)

enum Rhi_Access_Flags {
    RHI_ACCESS_INDIRECT_COMMAND_READ          = 0x00001,
    RHI_ACCESS_INDEX_READ                     = 0x00002,
    RHI_ACCESS_VERTEX_ATTRIBUTE_READ          = 0x00004,
    RHI_ACCESS_UNIFORM_READ                   = 0x00008,
    RHI_ACCESS_INPUT_ATTACHMENT_READ          = 0x00010,
    RHI_ACCESS_SHADER_READ                    = 0x00020,
    RHI_ACCESS_SHADER_WRITE                   = 0x00040,
    RHI_ACCESS_COLOR_ATTACHMENT_READ          = 0x00080,
    RHI_ACCESS_COLOR_ATTACHMENT_WRITE         = 0x00100,
    RHI_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ  = 0x00200,
    RHI_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00400,
    RHI_ACCESS_TRANSFER_READ                  = 0x00800,
    RHI_ACCESS_TRANSFER_WRITE                 = 0x01000,
    RHI_ACCESS_HOST_READ                      = 0x02000,
    RHI_ACCESS_HOST_WRITE                     = 0x04000,
    RHI_ACCESS_MEMORY_READ                    = 0x08000,
    RHI_ACCESS_MEMORY_WRITE                   = 0x10000,
};

typedef u32 Rhi_Pipeline_Stage_Mask;
typedef u32 Rhi_Access_Mask;

struct Rhi_Subpass_Dependency {
    u32 src_subpass = 0;
    u32 dst_subpass = 0;
    
    Rhi_Pipeline_Stage_Mask src_stages = 0;
    Rhi_Pipeline_Stage_Mask dst_stages = 0;
    
    Rhi_Access_Mask src_access = 0;
    Rhi_Access_Mask dst_access = 0;
};

struct Rhi_Render_Pass_Description {
    Array<Rhi_Attachment> attachments = {};
    Array<Rhi_Subpass> subpasses = {};
    Array<Rhi_Subpass_Dependency> deps = {};
};

enum Rhi_Subpass_Contents {
    RHI_SUBPASS_CONTENTS_INLINE = 0,
    RHI_SUBPASS_CONTENTS_EXTERNAL = 1,
};

struct Rhi_Depth_Stencil_Value {
    f32 depth = 0;
    u8 stencil = 0;
};

struct Rhi_Clear_Value {
    Rhi_Format format = RHI_FORMAT_UNDEFINED;
    f32 clear_color[4] = {};
    Rhi_Depth_Stencil_Value depth_stencil = {};
};

struct Rhi_Rect {
    s32 x;
    s32 y;
    u32 width;
    u32 height;
};

struct Rhi_Render_Pass_Info {
    Rhi_Rect render_area = {};
    Array<Rhi_Clear_Value> clear_values = {};
    Ref<Rhi_Render_Target> target;
};

struct Rhi_Render_Target_Description {
    bool is_swapchain_target = false;
};

struct Rhi_Sync_Object {
    virtual ~Rhi_Sync_Object() {}
};

struct Rhi_Device {
    virtual ~Rhi_Device() {}

    virtual Ref<Rhi_Command_List> create_command_list() = 0;
    virtual void wait_for_idle() = 0;
    virtual void new_frame() = 0;
    virtual void present() = 0;
    virtual void submit(const Array<Ref<Rhi_Command_List>> &lists, const Array<Ref<Rhi_Sync_Object>> &waits, const Array<Ref<Rhi_Sync_Object>> &signals) = 0;
    virtual void manual_shutdown() = 0;
    virtual Ref<Rhi_Sync_Object> create_sync_object(Rhi_Pipeline_Stage_Mask wait_stages = 0) = 0;
    virtual Ref<Rhi_Render_Pass> create_render_pass(const Rhi_Render_Pass_Description &desc) = 0;
    virtual Ref<Rhi_Render_Target> create_swapchain_target(Ref<Rhi_Render_Pass> pass) = 0;
    virtual Rhi_Format get_swapchain_image_format() = 0;
    virtual void notify_window_resize() = 0;
    virtual void get_extent(s32 *width, s32 *height) = 0;

    virtual bool is_active() = 0;
};

struct Rhi_Command_List {
    virtual ~Rhi_Command_List() {}

    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void manual_shutdown() = 0;
    virtual void begin_render_pass(Ref<Rhi_Render_Pass> render_pass, Rhi_Subpass_Contents contents, const Rhi_Render_Pass_Info &info) = 0;
    virtual void next_subpass(Rhi_Subpass_Contents contents) = 0;
    virtual void end_render_pass() = 0;
    virtual void render_imgui_draw_data(ImDrawData *data) = 0;
};

struct Rhi_Render_Pass {
    virtual ~Rhi_Render_Pass() {}
};

struct Rhi_Render_Target {
    virtual ~Rhi_Render_Target() {}
};

void init_rhi(Rhi_Vendor vendor, const String &app_name);
void switch_rhi(Rhi_Vendor vendor, const String &app_name);
void shutdown_rhi();
void init_imgui_rhi(Ref<Window> window, Ref<Rhi_Render_Pass> render_pass);
void shutdown_imgui_rhi();
void new_imgui_frame_rhi();

Ref<Rhi_Device> create_rhi_device(Window *window);
