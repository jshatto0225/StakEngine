#include "../../StakEngine/Source/Core/EntryPoint.h"

#include "Editor.h"

void initialize_application(Application *out_app, EngineSpecification *out_spec) {
    out_spec->window_width = 1920;
    out_spec->window_height = 1080;
    out_spec->window_title = "Stak Editor";
    out_spec->app_name = "StakEditor";
    out_spec->render_to_offscreen_buffer = true;

    auto editor = new Editor;
    out_app->init = init;
    out_app->shutdown = shutdown;
    out_app->on_imgui_render = on_imgui_render;
    out_app->user_data = editor;
}

void destroy_application(Application *app) {
    auto editor = static_cast<Editor *>(app->user_data);
    delete editor;
}

bool init(Engine *engine, Application *app) {
    auto editor = static_cast<Editor *>(app->user_data);

    editor->escape_pressed_callback_info = add_key_press_callback(&engine->input, KeyCode::Escape, [engine]() {
        LOG_TRACE("[Callback Input] Escape pressed. Quitting.");
        engine_close(engine);
    });
    editor->w_pressed_callback_info = add_key_press_callback(&engine->input, KeyCode::W, []() {
        LOG_TRACE("[Callback Input] W pressed");
    });

    return true;
}

void shutdown(Engine *engine, Application *app) {
    auto editor = static_cast<Editor *>(app->user_data);

    RemoveCallback(&engine->input, editor->escape_pressed_callback_info);
    RemoveCallback(&engine->input, editor->w_pressed_callback_info);
}

void on_imgui_render(Engine *engine, Application *app) {
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
            renderer_add_scene_to_imgui_window(&engine->renderer);
        }
        ImGui::End();
    }
    ImGui::End();
}

void update(Engine *engine, Application *app) {
    auto editor = static_cast<Editor *>(app->user_data);

    if (engine->input.keyboard[KeyCode::Space] == InputState::Down) {
        LOG_TRACE("[Polled Input] Space Pressed");
    }
}

