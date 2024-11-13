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

  mGraphicsContext = mRenderer->createGraphicsContext();

  mRenderer->initImGui();
}

ImGuiLayer::~ImGuiLayer() {
  mRenderer->shutdownImGui();
  mWindow->shutdownImGui();

  mRenderer = NULL;
  mWindow = NULL;
}

void ImGuiLayer::beginFrame() {
  mRenderer->imGuiNewFrame();
  mWindow->imGuiNewFrame();
  ImGui::NewFrame();
}

void ImGuiLayer::endFrame() {
  ImGui::Render();
  ImDrawData *drawData = ImGui::GetDrawData();
  const bool isMinimized = (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f);
  if (!isMinimized) {
    mGraphicsContext->begin();
    mRenderer->renderImGuiDrawData(drawData, mGraphicsContext);
    mGraphicsContext->end();
  }
}

void ImGuiLayer::onEvent(const IEvent &event) {
  
}

} // namespace Stak
