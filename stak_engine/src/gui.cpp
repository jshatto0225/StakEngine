#include "gui.hpp"

#include <imgui.h>

#include "rhi.hpp"

static Ref<Rhi_Render_Pass> render_pass = nullptr;
static Ref<Rhi_Render_Target> target = nullptr;
static ImDrawData *draw_data = nullptr;
static Ref<Rhi_Device> device = nullptr;

void init_imgui(Ref<Window> window) {
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
    
    device = window->get_device();
    
    Rhi_Attachment color_attachment = {};
    color_attachment.format           = device->get_swapchain_image_format();
    color_attachment.samples          = RHI_SAMPLE_COUNT_1;
    color_attachment.load_op          = RHI_LOAD_OP_CLEAR;
    color_attachment.store_op         = RHI_STORE_OP_STORE;
    color_attachment.stencil_load_op  = RHI_LOAD_OP_DONT_CARE;
    color_attachment.stencil_store_op = RHI_STORE_OP_DONT_CARE;
    color_attachment.initial_layout   = RHI_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.final_layout     = RHI_IMAGE_LAYOUT_PRESENT_SRC;

    Rhi_Attachment_Ref color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout     = RHI_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    Rhi_Subpass subpass = {};
    subpass.pipeline_bind_point = RHI_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.color_attachments   = { color_attachment_ref };

    Rhi_Subpass_Dependency dependency = {};
    dependency.src_subpass = RHI_SUBPASS_EXTERNAL;
    dependency.dst_subpass = 0;
    dependency.src_stages  = RHI_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
    dependency.src_access  = 0;
    dependency.dst_stages  = RHI_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
    dependency.dst_access  = RHI_ACCESS_COLOR_ATTACHMENT_WRITE;

    Rhi_Render_Pass_Description desc = {};
    desc.attachments = { color_attachment };
    desc.subpasses   = { subpass };
    desc.deps        = { dependency };
    
    render_pass = device->create_render_pass(desc);
    ASSERT(render_pass != nullptr);

    target = device->create_swapchain_target(render_pass);
    ASSERT(target != nullptr);
    
    init_imgui_rhi(window, render_pass);
}
    
void shutdown_imgui() {
    shutdown_imgui_rhi();
    ImGui::DestroyContext();
}
    
void begin_imgui_frame() {
    new_imgui_frame_rhi();
    ImGui::NewFrame();
}
    
void end_imgui_frame() {
    ImGui::Render();
    ImDrawData *data = ImGui::GetDrawData();
    const bool is_minimized = (data->DisplaySize.x <= 0.0f || data->DisplaySize.y <= 0.0f);
    if (!is_minimized)
        draw_data = data;
}

void render_imgui(Ref<Rhi_Command_List> command_list) {    
    // Gui render pass
    Rhi_Rect render_area = {};
    render_area.x      = 0;
    render_area.y      = 0;

    s32 width, height;
    device->get_extent(&width, &height);
    render_area.width = width;
    render_area.height = height;
        
    Rhi_Clear_Value clear_value = {};
    clear_value.format         = RHI_FORMAT_R32G32B32A32_SFLOAT;
    clear_value.clear_color[0] = 1.0f;
    clear_value.clear_color[1] = 0.0f;
    clear_value.clear_color[2] = 1.0f;
    clear_value.clear_color[3] = 1.0f;
        
    Rhi_Render_Pass_Info info = {};
    info.render_area  = render_area;
    info.clear_values = { clear_value };
    info.target       = target;

    command_list->begin_render_pass(render_pass, RHI_SUBPASS_CONTENTS_INLINE, info);

    if (draw_data) {
        command_list->render_imgui_draw_data(draw_data);
    }

    command_list->end_render_pass();
}
