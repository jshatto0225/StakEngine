#include "EditorLayer.h"

EditorLayer::EditorLayer(Editor *editor) : m_Editor(editor) {}

void EditorLayer::update() {
  if (Stak::Input::keyDown(Stak::KeyCode::ESCAPE)) {
    LOG_TRACE("Escape pressed. Quitting.");
    m_Editor->close();
  }
}
