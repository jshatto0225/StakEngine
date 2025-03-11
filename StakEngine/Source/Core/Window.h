#pragma once

#include <functional>

#include "Events.h"
#include "Types.h"
#include "RHIViewport.h"

class FRHIDevice;

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

class IWindow {
public:
    virtual bool Init(const FWindowConfig &Cfg) = 0;
    virtual void Shutdown() = 0;

    virtual FWindowSizeData GetSize() = 0;
    virtual FWindowPosData GetPos() = 0;

    using FEventFn  = std::function<void(const FEvent &)>;
    virtual void SetEventFn(const FEventFn &Func) = 0;

    virtual void InitImGui() = 0;
    virtual void ImGuiNewFrame() = 0;
    virtual void ShutdownImGui() = 0;

    virtual FWindowSizeData GetFramebufferSize() = 0;

    virtual TRef<IRHIViewport> GetRHIViewport() = 0;

    static TRef<IWindow> Create();
};
