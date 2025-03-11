#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "Window.h"
#include "Renderer.h"

struct FImGuiRenderer final : public FRenderProxy {
    void BeginFrame();
    void EndFrame();

    bool Init(FRenderer *Renderer);
    void Shutdown();

    void Render(TRef<IRHICommandContext> Context) override;

    ImDrawData *DrawData;
    FRenderer *Renderer;
};
