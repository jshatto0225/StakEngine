#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "EditorLayer.h"

FApplication *CreateApplication() {
  FApplicationSpec Spec = {};
  Spec.WindowWidth = 1920;
  Spec.WindowHeight = 1080;
  Spec.WindowTitle = "Stak Editor";

  FApplication *Editor = new FApplication(Spec);
  Editor->AddLayer(new FEditorLayer());

  return Editor;
}
