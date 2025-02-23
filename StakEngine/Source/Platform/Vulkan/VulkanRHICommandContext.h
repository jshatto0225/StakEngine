#pragma once

#include "RHICommandContext.h"
#include "VulkanRHI.h"
#include "FLog.h"

class Vulkan_Rhi_Command_Context final : public Rhi_Command_Context {
public:

    inline void begin_drawing() override {
        active = true;
        Rhi::get().begin_drawing();
    }
    
    inline void end_drawing() override {
        active = false;
        Rhi::get().end_drawing();
    }
    
    inline void resource_transition(Rhi_Transition *transition) override {
        SK_LOG_ERROR("Resource transitions not implemented");
    }
    
    inline void render_ingui_draw_data(ImDrawData *draw_data) override {
        Rhi::get().submit_imgui_draw_data(draw_data);
    }
    
    inline bool is_active() override { return active; }
    
private:
    bool active = false;
};
