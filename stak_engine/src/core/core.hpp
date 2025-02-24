#pragma once

#include <vector>
#include <string>

#include "platform/platform.hpp"
#include "renderer/renderer.hpp"
#include "rhi/rhi.hpp"

namespace Core {

struct App {
    std::string name = "";
    Platform::Window_Config window_config = {};
    Rhi::Vendor renderer_vendor = Rhi::Vendor::NONE;

    void (*init)() = nullptr;
    void (*update)() = nullptr;
    void (*shutdown)() = nullptr;
    void (*gui_render)() = nullptr;
};

void run(App *app);
void close();

}
