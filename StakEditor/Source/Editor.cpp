#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "Editor.h"

Application *create_application() {
    auto app = (Application *) malloc(sizeof(Application));
    if (!app) {
        return nullptr;
    }

    memset(app, 0, sizeof(Application));

    auto editor = (Editor *) malloc(sizeof(Editor));
    if (!editor) {
        free(app);
        return nullptr;
    }

    app->init = init;
    app->on_imgui_render = on_imgui_render;
    app->get_engine_spec = get_engine_spec;
    app->user_data = editor;

    return app;
}

EngineSpecification get_engine_spec(Application *app) {
    return {
        "Stak Editor",
        1920,
        1080,
    };
}

void destroy_application(Application *app, Engine *engine) {
    auto editor = static_cast<Editor *>(app->user_data);

    remove_callback(engine->input, editor->escape_pressed_callback_info);
    remove_callback(engine->input, editor->w_pressed_callback_info);

    free(editor);
    free(app);
}

bool init(Application *app, Engine *engine) {
    auto editor = static_cast<Editor *>(app->user_data);

    editor->escape_pressed_callback_info = add_key_press_callback(engine->input, KeyCode::Escape, [engine]() {
        LOG_TRACE("[Callback Input] Escape pressed. Quitting.");
        engine_close(engine);
    });
    editor->w_pressed_callback_info = add_key_press_callback(engine->input, KeyCode::W, []() {
        LOG_TRACE("[Callback Input] W pressed");
    });

    return true;
}

void on_imgui_render(Application *app, Engine *engine) {
    auto editor = static_cast<Editor *>(app->user_data);

    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    {
        if (!opt_padding) ImGui::PopStyleVar();
        if (opt_fullscreen) ImGui::PopStyleVar(2);

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                ImGui::MenuItem("Fullscreen", nullptr, &opt_fullscreen);
                ImGui::MenuItem("Padding", nullptr, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
                }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit;
                }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking;
                }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::Begin("Test Window");
        {

        }
        ImGui::End();

        ImGui::Begin("Scene View");
        {
        }
        ImGui::End();
    }
    ImGui::End();
}

void update(Application *app, Engine *engine) {
    auto editor = static_cast<Editor *>(app->user_data);

    if (get_key(engine->input, KeyCode::Space) == InputState::Down) {
        LOG_TRACE("[Polled Input] Space Pressed");
    }
}

