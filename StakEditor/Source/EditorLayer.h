#pragma once

#include "../StakEngine/StakEngine.h"

#include "Editor.h"

class EditorLayer : public Stak::ApplicationLayer {
public:
  EditorLayer(Editor *editor);
  void Update();
private:
  Editor *m_Editor;
};
