#include "ImguiLayer.h"

#include <imgui.h>

#include "Log.h"
#include "Renderer.h"
#include "Application.h"

FImGuiLayer::FImGuiLayer() {
    DrawData = nullptr;

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();
}

FImGuiLayer::~FImGuiLayer() {
    ImGui::DestroyContext();
}

void FImGuiLayer::BeginFrame() {
    ImGui::NewFrame();
}

void FImGuiLayer::EndFrame() {
    ImGui::Render();
    ImDrawData *Data = ImGui::GetDrawData();
    const FBool IsMinimized = (Data->DisplaySize.x <= 0.0f || Data->DisplaySize.y <= 0.0f);
    if (!IsMinimized) {
        DrawData = Data;
    }
}

void FImGuiLayer::Render(TRef<IRHICommandContext> CommandContext) {
    CommandContext->RenderImGuiDrawData(DrawData);
}
