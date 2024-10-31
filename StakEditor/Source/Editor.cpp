#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "EditorLayer.h"

Editor::Editor(Stak::ApplicationSpec &spec) : Stak::Application(spec) {
  AddLayer(new EditorLayer(this));
  LOG_TRACE("Hello from editor");
}

Stak::Application *CreateApplication() {
  Stak::ApplicationSpec spec = {};
  spec.WindowWidth = 1920;
  spec.WindowHeight = 1080;
  spec.WindowTitle = "Stak Editor";

  return new Editor(spec);
}
