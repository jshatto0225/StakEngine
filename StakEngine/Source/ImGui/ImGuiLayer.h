#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "ApplicationLayer.h"
#include "Window.h"
#include "Renderer.h"

namespace Stak {

class ImGuiLayer : public ApplicationLayer {
public:
  ImGuiLayer(Ref<Window> window, Ref<Renderer> renderer);
  ~ImGuiLayer();

  void update();
  void onEvent(Event &event);
  void beginFrame();
  void endFrame();

private:
  void FrameRender(ImDrawData *drawData);
  void FramePresent();

private:
  Ref<Window> mWindow;
  Ref<Renderer> mRenderer;
};

} // namespace Stak
