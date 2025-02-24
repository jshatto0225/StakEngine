#include "stak_engine.hpp"

int main() {
	Core::App app = {};
    
    app.renderer_vendor = Rhi::Vendor::VULKAN;

    app.window_config.x = 100;
    app.window_config.y = 100;
    app.window_config.width = 1920;
    app.window_config.height = 1080;
    app.window_config.title = "Stak Editor";
    
    app.name = "Stak Editor";

	Core::run(&app);
}
