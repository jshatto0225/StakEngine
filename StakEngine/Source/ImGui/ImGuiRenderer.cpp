#include "ImGuiRenderer.h"

#include <imgui.h>

#include "Log.h"
#include "Renderer.h"
#include "Engine.h"

bool imgui_renderer_init(ImguiRenderer *imgui_renderer, Renderer *renderer) {
    imgui_renderer->renderer = renderer;
    imgui_renderer->draw_data = nullptr;

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    return true;
}

void imgui_renderer_shutdown(ImguiRenderer *imgui_renderer) {
    ImGui::DestroyContext();
}

void imgui_renderer_begin_frame(ImguiRenderer *imgui_renderer) {
    ImGui::NewFrame();
}

void imgui_renderer_end_frame(ImguiRenderer *imgui_renderer) {
    ImGui::Render();
    ImDrawData *data = ImGui::GetDrawData();
    const bool minimized = (data->DisplaySize.x <= 0.0f || data->DisplaySize.y <= 0.0f);
    if (!minimized) {
        imgui_renderer->draw_data = data;
    }
}

