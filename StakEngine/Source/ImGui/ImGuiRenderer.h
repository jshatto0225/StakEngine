#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "Window.h"
#include "Renderer.h"

struct FImGuiRenderer {
    ImDrawData *DrawData;
    FRenderer *Renderer;
};

void ImGuiRendererBeginFrame(FImGuiRenderer *ImGuiRenderer);

void ImGuiRendererEndFrame(FImGuiRenderer *ImGuiRenderer);

bool ImGuiRendererInit(FImGuiRenderer *ImGuiRenderer, FRenderer *Renderer);

void ImGuiRendererShutdown(FImGuiRenderer *ImGuiRenderer);