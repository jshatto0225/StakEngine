#include "../StakEngine/EntryPoint.h"

#include "EditorLayer.h"

Application::spec GetAppSpec()
{
    Application::spec Spec = {};
    Spec.WindowWidth = 1920;
    Spec.WindowHeight = 1080;
    Spec.WindowX = 0;
    Spec.WindowY = 0;
    Spec.WindowTitle = "Stak Editor";

    Application::AddLayerToStack(EditorLayer::Init, EditorLayer::Shutdown, EditorLayer::Update, EditorLayer::OnEvent);
    return Spec;
}
