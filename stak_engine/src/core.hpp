#pragma once

#include "platform.hpp"
#include "renderer.hpp"
#include "rhi.hpp"
#include "utils.hpp"
#include "window.hpp"

struct App {
    String name = "";
    Window_Config window_config = {};
    Rhi_Vendor renderer_vendor = Rhi_Vendor::NONE;

    // TODO: Dynamically load these
    void (*init)() = nullptr;
    void (*update)() = nullptr;
    void (*shutdown)() = nullptr;
    void (*imgui_render)() = nullptr;
};

void run_app(App &app);
void close_app();
