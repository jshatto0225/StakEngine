#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "EditorLayer.h"

App *create_application() {
    App_Spec spec = {};
    spec.window_width = 1920;
    spec.window_height = 1080;
    spec.window_title = "Stak Editor";

    App *editor = new App(spec);
    editor->add_layer(new Editor_Layer());

    return editor;
}
