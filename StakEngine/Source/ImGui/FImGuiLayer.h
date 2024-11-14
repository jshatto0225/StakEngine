#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "IApplicationLayer.h"
#include "IWindow.h"
#include "Renderer.h"

class FImGuiLayer final : public IApplicationLayer {
public:
  FImGuiLayer();
  ~FImGuiLayer();

  void BeginFrame();
  void EndFrame();
};
