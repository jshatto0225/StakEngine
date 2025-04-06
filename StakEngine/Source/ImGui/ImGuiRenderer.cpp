#include "ImGuiRenderer.h"

#include <imgui.h>

#include "Log.h"
#include "Renderer.h"
#include "Engine.h"

bool imgui_renderer_init(Imgui_Renderer *imgui_renderer, Renderer *renderer) {
    imgui_renderer->renderer = renderer;
    imgui_renderer->draw_data = nullptr;

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    renderer->gui_render_func = [imgui_renderer] (Rhi_Resource_Handle List) {
        rhi.cmd_render_imgui_draw_data(List, imgui_renderer->draw_data);
    };

    if (!renderer_init_imgui(imgui_renderer->renderer)) {
        SK_LOG_ERROR("Failed to initialize renderer for imgui");
        return false;
    }

    return true;
}

void imgui_renderer_shutdown(Imgui_Renderer *imgui_renderer) {
    renderer_shutdown_imgui(imgui_renderer->renderer);
    ImGui::DestroyContext();
}

void imgui_renderer_begin_frame(Imgui_Renderer *imgui_renderer) {
    renderer_imgui_new_frame(imgui_renderer->renderer);
    ImGui::NewFrame();
}

void imgui_renderer_end_frame(Imgui_Renderer *imgui_renderer) {
    ImGui::Render();
    ImDrawData *data = ImGui::GetDrawData();
    const bool minimized = (data->DisplaySize.x <= 0.0f || data->DisplaySize.y <= 0.0f);
    if (!minimized) {
        imgui_renderer->draw_data = data;
    }
}

