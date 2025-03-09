#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "EditorLayer.h"

bool InitializeApplication(FApplication *Editor) {
    FApplicationSpec Spec = {};
    Spec.WindowWidth = 1920;
    Spec.WindowHeight = 1080;
    Spec.WindowTitle = "Stak Editor";
    Spec.AppName = "StakEditor";
    Spec.RenderToOffscreenBuffer = true;

    if (!Editor->Init(Spec)) {
        LOG_ERROR("Failed to initialize application");
        return false;
    }

    Editor->AddLayer(new FEditorLayer());

    return true;
}
