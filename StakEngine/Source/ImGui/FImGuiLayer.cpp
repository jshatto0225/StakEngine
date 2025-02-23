#include "FImguiLayer.h"

#include <imgui.h>

#include "FLog.h"
#include "FRenderer.h"

Imui_Layer::Imui_Layer() {
    draw_data = nullptr;

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    Renderer::get().init_imgui();
}

Imui_Layer::~Imui_Layer() {
    Renderer::get().shutdown_imgui();
    ImGui::DestroyContext();
}

void Imui_Layer::begin_frame() {
    Renderer::get().imgui_new_frame();
    ImGui::NewFrame();
}

void Imui_Layer::end_frame() {
    ImGui::Render();
    ImDrawData *data = ImGui::GetDrawData();
    const bool is_minimized = (data->DisplaySize.x <= 0.0f || data->DisplaySize.y <= 0.0f);
    if (!is_minimized) {
        draw_data = data;
    }
}

void Imui_Layer::on_window_resize(const Window_Resize_Event &event) {

}

void Imui_Layer::render(Rhi_Command_List &command_list) {
    command_list.render_imgui_draw_data(draw_data);
}
