#include "ImGuiRenderer.h"

#include <imgui.h>

#include "Log.h"
#include "Renderer.h"
#include "Application.h"

bool FImGuiRenderer::Init(FRenderer *R) {
    Renderer = R;
    DrawData = nullptr;

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    Renderer->SetPostProxy(this);
    if (!Renderer->InitImGui()) {
        SK_LOG_ERROR("Failed to initialize renderer for imgui");
        return false;
    }

    return true;
}

void FImGuiRenderer::Shutdown() {
    Renderer->ShutdownImGui();
    ImGui::DestroyContext();
}

void FImGuiRenderer::BeginFrame() {
    Renderer->ImGuiNewFrame();
    ImGui::NewFrame();
}

void FImGuiRenderer::EndFrame() {
    ImGui::Render();
    ImDrawData *Data = ImGui::GetDrawData();
    const FBool IsMinimized = (Data->DisplaySize.x <= 0.0f || Data->DisplaySize.y <= 0.0f);
    if (!IsMinimized) {
        DrawData = Data;
    }
}

void FImGuiRenderer::Render(TRef<IRHICommandContext> CommandContext) {
    CommandContext->RenderImGuiDrawData(DrawData);
}
