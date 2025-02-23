#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#include "IApplicationLayer.h"
#include "IWindow.h"
#include "FRenderer.h"

class Imui_Layer final : public App_Layer, public Render_Proxy {
public:
    Imui_Layer();
    ~Imui_Layer();

    void begin_frame();
    void end_frame();
    void on_window_resize(const Window_Resize_Event &event) override;

    void render(Rhi_Command_List &context) override;

private:
    ImDrawData *draw_data;
};
