#include "EditorLayer.h"

EditorLayer::EditorLayer(Editor *editor) : m_Editor(editor) {}

void EditorLayer::Update() {
  if (m_Editor->Input()->KeyDown(Stak::KeyCode::ESCAPE)) {
    LOG_TRACE("Escape pressed. Quitting.");
    m_Editor->Close();
  }
}
