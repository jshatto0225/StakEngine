#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "EditorLayer.h"

TRef<FApplication> CreateApplication() {
  FApplicationSpec Spec = {};
  Spec.WindowWidth = 1920;
  Spec.WindowHeight = 1080;
  Spec.WindowTitle = "Stak Editor";

  TRef<FApplication> Editor = TCreateRef<FApplication>(Spec);
  Editor->AddLayer(new FEditorLayer(Editor->GetInput()));

  return Editor;
}
