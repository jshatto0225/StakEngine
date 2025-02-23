#include "EditorLayer.h"

Editor_Layer::Editor_Layer() {
  Input::get().add_key_state_callback(Input_State::DOWN, Key_Code::ESCAPE, []() {
      LOG_TRACE("Escape pressed. Quitting.");
      App::get().close();
  });
  Input::get().add_key_state_callback(Input_State::DOWN, Key_Code::W, []() {
      LOG_TRACE("W pressed");
  });
}

void Editor_Layer::on_imgui_render() {
    ImGui::Begin("Test Window");
    {
        ImGui::Text("Hello, world! This is a test ImGui window.");
        if (ImGui::Button("Click Me")) {
            LOG_TRACE("Button was clicked!");
        }
    }
    ImGui::End();
}

void Editor_Layer::update() {
    if (Input::get().get_key(Key_Code::SPACE) == Input_State::DOWN) {
        LOG_TRACE("Space Pressed");
    }
}
