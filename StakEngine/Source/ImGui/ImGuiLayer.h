#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "ApplicationLayer.h"
#include "Window.h"
#include "Renderer.h"

class FImGuiLayer final : public IApplicationLayer, public FRenderProxy {
public:
    FImGuiLayer();
    ~FImGuiLayer();

    void BeginFrame();
    void EndFrame();

    void Render(FRHICommandContext &Context) override;

private:
    ImDrawData *DrawData;
};
