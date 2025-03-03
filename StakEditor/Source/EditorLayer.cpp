#include "EditorLayer.h"

FEditorLayer::FEditorLayer() {
    AppGetInput()->AddKeyStateCallback(EInputState::DOWN, EKeyCode::ESCAPE, []() {
        LOG_TRACE("Escape pressed. Quitting.");
        AppClose();
    });
    AppGetInput()->AddKeyStateCallback(EInputState::DOWN, EKeyCode::W, []() {
        LOG_TRACE("W pressed");
    });
}

void FEditorLayer::OnImGuiRender() {
    ImGui::Begin("Test Window");
    ImGui::Text("Hello, world! This is a test ImGui window.");
    if (ImGui::Button("Click Me")) {
        LOG_TRACE("Button was clicked!");
    }
    ImGui::End();
}

void FEditorLayer::Update() {
    if (AppGetInput()->GetKey(EKeyCode::SPACE) == EInputState::DOWN) {
        LOG_TRACE("Space Pressed");
    }
}
