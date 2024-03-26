#include "../StakEngine/StakEngine.h"
#include "../StakEngine/EntryPoint.h"

#include "EditorLayer.h"

application_spec GetAppSpec() {
  application_spec Spec = {};
  Spec.WindowWidth = 1280;
  Spec.WindowHeight = 720;
  Spec.WindowX = 0;
  Spec.WindowY = 0;
  Spec.WindowTitle = "Stak Editor";

  AddLayerToStack(&Spec.Layers,
                  EditorLayerInit,
                  EditorLayerShutdown,
                  EditorLayerUpdate,
                  EditorLayerOnEvent);
  return Spec;
}
