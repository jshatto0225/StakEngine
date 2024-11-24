#include "FImguiLayer.h"

#include <imgui.h>

#include "FLog.h"
#include "FRenderer.h"

FImGuiLayer::FImGuiLayer(TRef<FRenderer> Renderer) {
  mRenderer = Renderer;

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();

  mRenderer->InitImGui();
}

FImGuiLayer::~FImGuiLayer() {
  mRenderer->ShutdownImGui();
  ImGui::DestroyContext();
}

void FImGuiLayer::BeginFrame() {
  mRenderer->ImGuiNewFrame();
  ImGui::NewFrame();
}

void FImGuiLayer::EndFrame() {
  ImGui::Render();
  ImDrawData *DrawData = ImGui::GetDrawData();
  const FBool IsMinimized = (DrawData->DisplaySize.x <= 0.0f || DrawData->DisplaySize.y <= 0.0f);
  if (!IsMinimized) {
    mRenderer->SubmitImGuiDrawData(DrawData);
  }
}

void FImGuiLayer::OnWindowResize(const FWindowResizeEvent &Event) {

}
