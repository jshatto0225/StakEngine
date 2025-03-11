#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "ApplicationLayer.h"
#include "Window.h"
#include "Renderer.h"

class FImGuiLayer final : public IApplicationLayer, public FRenderProxy {
public:
    void BeginFrame();
    void EndFrame();

    void OnAttach() override;
    void OnDetach() override;

    void Render(TRef<IRHICommandContext> Context) override;

private:
    ImDrawData *DrawData;
};
