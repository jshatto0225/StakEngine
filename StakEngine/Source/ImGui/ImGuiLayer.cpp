#include "ImguiLayer.h"

#include <imgui.h>

#include "Renderer.h"
#include "Log.h"

namespace Stak {

ImGuiLayer::ImGuiLayer(Ref<Window> window, Ref<Renderer> renderer) : mWindow(window), mRenderer(renderer) {
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();

  window->initImGui();
  renderer->initImGui();
}

ImGuiLayer::~ImGuiLayer() {

}

void ImGuiLayer::beginFrame() {
  mRenderer->imGuiNewFrame();
  mWindow->imGuiNewFrame();
  ImGui::NewFrame();
}

void ImGuiLayer::endFrame() {
  // Rendering
  ImGui::Render();
  ImDrawData *drawData = ImGui::GetDrawData();
  const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);

  if (!isMinimized) {
    FrameRender(drawData);
    FramePresent();
  }
}

void ImGuiLayer::FrameRender(ImDrawData *drawData) {

}

void ImGuiLayer::FramePresent() {

}

void ImGuiLayer::update() {

}

void ImGuiLayer::onEvent(Event &event) {

}

} // namespace Stak
