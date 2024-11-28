#include "FImguiLayer.h"

#include <imgui.h>

#include "FLog.h"
#include "FRenderer.h"

FImGuiLayer::FImGuiLayer() {
  mDrawData = NULL;

  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();

  FRenderer::Get().InitImGui();
}

FImGuiLayer::~FImGuiLayer() {
  FRenderer::Get().ShutdownImGui();
  ImGui::DestroyContext();
}

void FImGuiLayer::BeginFrame() {
  FRenderer::Get().ImGuiNewFrame();
  ImGui::NewFrame();
}

void FImGuiLayer::EndFrame() {
  ImGui::Render();
  ImDrawData *DrawData = ImGui::GetDrawData();
  const FBool IsMinimized = (DrawData->DisplaySize.x <= 0.0f || DrawData->DisplaySize.y <= 0.0f);
  if (!IsMinimized) {
    mDrawData = DrawData;
  }
}

void FImGuiLayer::OnWindowResize(const FWindowResizeEvent &Event) {

}

void FImGuiLayer::Render(FRHICommandList &CommandList) {
  CommandList.RenderImGuiDrawData(mDrawData);
}
