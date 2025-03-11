#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "Window.h"
#include "Renderer.h"

class FImGuiRenderer final : public FRenderProxy {
public:
    void BeginFrame();
    void EndFrame();

    bool Init(FRenderer *Renderer);
    void Shutdown();

    void Render(TRef<IRHICommandContext> Context) override;

private:
    ImDrawData *DrawData;
    FRenderer *Renderer;
};
