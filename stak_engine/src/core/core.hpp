#pragma once

#include "platform/platform.hpp"
#include "renderer/renderer.hpp"
#include "rhi/rhi.hpp"
#include "utils/utils.hpp"

namespace Core {

struct App {
    String name = "";
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
