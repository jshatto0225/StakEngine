#include "EditorLayer.h"

void FEditorLayer::OnAttach() {
    EscapePressedCallbackInfo = AppGetInput()->AddKeyPressCallback(EKeyCode::ESCAPE, []() {
        LOG_TRACE("[Callback Input] Escape pressed. Quitting.");
        AppClose();
    });
    WPressedCallbackInfo = AppGetInput()->AddKeyPressCallback(EKeyCode::W, []() {
        LOG_TRACE("[Callback Input] W pressed");
    });
}

void FEditorLayer::OnDetach() {
    AppGetInput()->RemoveCallback(EscapePressedCallbackInfo);
    AppGetInput()->RemoveCallback(WPressedCallbackInfo);
}

void FEditorLayer::OnImGuiRender() {
    ImGui::Begin("Test Window");
    ImGui::Text("Hello, world! This is a test ImGui window.");
    if (ImGui::Button("Click Me")) {
        LOG_TRACE("ImGui Button was clicked!");
    }
    ImGui::End();
}

void FEditorLayer::Update() {
    if (AppGetInput()->GetKey(EKeyCode::SPACE) == EInputState::DOWN) {
        LOG_TRACE("[Polled Input] Space Pressed");
    }
}
