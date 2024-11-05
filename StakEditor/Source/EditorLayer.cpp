#include "EditorLayer.h"

EditorLayer::EditorLayer(Editor *editor) : m_Editor(editor) {}

void EditorLayer::Update() {
  if (Stak::Input::KeyDown(Stak::KeyCode::ESCAPE)) {
    LOG_TRACE("Escape pressed. Quitting.");
    m_Editor->Close();
  }
}
