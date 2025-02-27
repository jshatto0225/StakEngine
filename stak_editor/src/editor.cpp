#include "stak_engine.hpp"

#include <imgui.h>

void imgui_render() {
    ImGui::Begin("Test Window");
    {
        ImGui::Text("Hello, world! This is a test ImGui window.");
        if (ImGui::Button("Click Me")) {
            LOG_TRACE("Button was clicked!");
        }
    }
    ImGui::End();
}

int main() {
	App app = {};
    
    app.renderer_vendor = Rhi_Vendor::VULKAN;

    app.window_config.x = 100;
    app.window_config.y = 100;
    app.window_config.width = 1920;
    app.window_config.height = 1080;
    app.window_config.title = "Stak Editor";
    
    app.name = "Stak Editor";

    app.imgui_render = imgui_render;

	run_app(app);
}

