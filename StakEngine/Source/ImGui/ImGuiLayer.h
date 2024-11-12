#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "ApplicationLayer.h"
#include "Window.h"
#include "Renderer.h"

namespace Stak {

class ImGuiLayer : public IApplicationLayer {
public:
  ImGuiLayer(Ref<Window> window, Ref<Renderer> renderer);
  ~ImGuiLayer();

  void onEvent(const IEvent &event);
  void beginFrame();
  void endFrame();

private:
  Ref<Window> mWindow;
  Ref<Renderer> mRenderer;
  Ref<IRHIGraphicsContext> mGraphicsContext;
};

} // namespace Stak
