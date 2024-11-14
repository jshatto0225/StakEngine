#include "EditorLayer.h"

FEditorLayer::FEditorLayer(TRef<const IInput> Input) {
  mInput = Input;
}

void FEditorLayer::OnImGuiRender() {
  // Create a basic ImGui window
  ImGui::Begin("Test Window");
  ImGui::Text("Hello, world! This is a test ImGui window.");
  if (ImGui::Button("Click Me")) {
    LOG_TRACE("Button was clicked!");
  }
  ImGui::End();
}

void FEditorLayer::Update() {
  if (mInput->KeyDown(EKeyCode::ESCAPE)) {
    LOG_TRACE("Escape pressed. Quitting.");
  }
}
