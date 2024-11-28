#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "IApplicationLayer.h"
#include "IWindow.h"
#include "FRenderer.h"

class FImGuiLayer final : public IApplicationLayer, public FRenderProxy {
public:
  FImGuiLayer();
  ~FImGuiLayer();

  void BeginFrame();
  void EndFrame();
  void OnWindowResize(const FWindowResizeEvent &Event) override;

  void Render(FRHICommandList &Context) override;

private:
  ImDrawData *mDrawData;
};
