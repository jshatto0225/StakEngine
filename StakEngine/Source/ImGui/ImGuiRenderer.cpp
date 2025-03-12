#include "ImGuiRenderer.h"

#include <imgui.h>

#include "Log.h"
#include "Renderer.h"
#include "Engine.h"

bool ImGuiRendererInit(FImGuiRenderer *ImGuiRenderer, FRenderer *Renderer) {
    ImGuiRenderer->Renderer = Renderer;
    ImGuiRenderer->DrawData = nullptr;

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    Renderer->GuiRenderFunc = [ImGuiRenderer] (TRef<IRHICommandContext> CommandContext) {
        CommandContext->RenderImGuiDrawData(ImGuiRenderer->DrawData);
    };

    if (!RendererInitImGui(ImGuiRenderer->Renderer)) {
        SK_LOG_ERROR("Failed to initialize renderer for imgui");
        return false;
    }

    return true;
}

void ImGuiRendererShutdown(FImGuiRenderer *ImGuiRenderer) {
    RendererShutdownImGui(ImGuiRenderer->Renderer);
    ImGui::DestroyContext();
}

void ImGuiRendererBeginFrame(FImGuiRenderer *ImGuiRenderer) {
    RendererImGuiNewFrame(ImGuiRenderer->Renderer);
    ImGui::NewFrame();
}

void ImGuiRendererEndFrame(FImGuiRenderer *ImGuiRenderer) {
    ImGui::Render();
    ImDrawData *Data = ImGui::GetDrawData();
    const FBool IsMinimized = (Data->DisplaySize.x <= 0.0f || Data->DisplaySize.y <= 0.0f);
    if (!IsMinimized) {
        ImGuiRenderer->DrawData = Data;
    }
}
