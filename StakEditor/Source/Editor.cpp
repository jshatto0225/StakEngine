#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "EditorLayer.h"

Editor::Editor(Stak::ApplicationSpec &spec) : Stak::Application(spec) {
  AddLayer(new EditorLayer(this));
  LOG_TRACE("Hello from editor");
}

Stak::Application *CreateApplication() {
  Stak::ApplicationSpec spec = {};
  spec.windowWidth = 1920;
  spec.windowHeight = 1080;
  spec.windowX = 0;
  spec.windowY = 0;
  spec.windowTitle = "Stak Editor";

  return new Editor(spec);
}
