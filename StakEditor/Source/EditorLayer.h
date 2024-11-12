#pragma once

#include <StakEngine.h>

#include "Editor.h"

class EditorLayer : public Stak::IApplicationLayer {
public:
  EditorLayer(Editor *editor);
  void update();
private:
  Editor *m_Editor;
};
