#pragma once

#include <functional>

#include "Events.h"
#include "Types.h"
#include "RHIViewport.h"

struct FWindowConfig {
    FSInt32 Width;
    FSInt32 Height;
    const char *Title;
};

struct FWindowSizeData {
    FSInt32 Width;
    FSInt32 Height;
};

struct FWindowPosData {
    FSInt32 X;
    FSInt32 Y;
};

struct IWindow {
    virtual bool Init(const FWindowConfig &Cfg) = 0;
    virtual void Shutdown() = 0;

    using FEventFn  = std::function<void(FEvent *)>;

    virtual void InitImGui() = 0;
    virtual void ImGuiNewFrame() = 0;
    virtual void ShutdownImGui() = 0;

    static TRef<IWindow> Create();

    struct FWindowData {
        FEventFn EventFn = nullptr;
        FSInt32 X = 0;
        FSInt32 Y = 0;
        FSInt32 Width = 0;
        FSInt32 Height = 0;
        FSInt32 FramebufferWidth = 0;
        FSInt32 FramebufferHeight = 0;
        std::string Title;
        TRef<IRHIViewport> Viewport = nullptr;
    };

    FWindowData Data = {};
};
