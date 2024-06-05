#include "../StakEngine/EntryPoint.h"

#include "EditorLayer.h"

application_spec GetAppSpec()
{
    application_spec Spec = {};
    Spec.WindowWidth = 1920;
    Spec.WindowHeight = 1080;
    Spec.WindowX = 0;
    Spec.WindowY = 0;
    Spec.WindowTitle = "Stak Editor";

    AddLayerToStack(EditorLayerInit, EditorLayerShutdown, EditorLayerUpdate, EditorLayerOnEvent);
    return Spec;
}
