#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "EditorLayer.h"

FApplication *CreateApplication() {
    FApplicationSpec Spec = {};
    Spec.WindowWidth = 1920;
    Spec.WindowHeight = 1080;
    Spec.WindowTitle = "Stak Editor";
    Spec.AppName = "StakEditor";

    auto Editor = new FApplication();

    if (!Editor->Init(Spec)) {
        LOG_ERROR("Failed to initialize application");
        return nullptr;
    }

    Editor->AddLayer(new FEditorLayer());

    return Editor;
}
