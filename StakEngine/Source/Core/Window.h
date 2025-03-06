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

    IWindow() = default;
    virtual ~IWindow() = default;

    virtual FWindowSizeData GetSize() = 0;
    virtual FWindowPosData GetPos() = 0;

    using FWindowResizeEventFn  = std::function<void(const FWindowResizeEvent &)>;
    using FKeyEventFn           = std::function<void(const FKeyEvent &)>;
    using FMouseButtonEventFn   = std::function<void(const FMouseButtonEvent &)>;
    using FMouseMoveEventFn     = std::function<void(const FMouseMoveEvent &)>;
    virtual void SetResizeEventFn(const FWindowResizeEventFn &Func) = 0;
    virtual void SetKeyEventFn(const FKeyEventFn &Func) = 0;
    virtual void SetMouseButtonEventFn(const FMouseButtonEventFn &Func) = 0;
    virtual void SetMouseMoveEventFn(const FMouseMoveEventFn &Func) = 0;

    using FWindowCloseEventFn = std::function<void()>;
    virtual void SetCloseEventFn(const FWindowCloseEventFn &Func) = 0;

    virtual void InitImGui() = 0;
    virtual void ImGuiNewFrame() = 0;
    virtual void ShutdownImGui() = 0;

    virtual FWindowSizeData GetFramebufferSize() = 0;

    virtual TRef<IRHIViewport> GetRHIViewport() = 0;

    static TScope<IWindow> Create(const FWindowConfig &Cfg);
};
