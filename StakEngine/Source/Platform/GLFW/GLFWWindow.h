#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "Window.h"
#include "Events.h"

#include "RHIDevice.h"

class FGLFWWindow final : public IWindow {
public:
    FGLFWWindow(const FWindowConfig &Cfg);
    ~FGLFWWindow() override;

    FWindowSizeData GetSize() override;
    FWindowPosData GetPos() override;

    void InitImGui() override;
    void ImGuiNewFrame() override;
    void ShutdownImGui() override;

    void SetResizeEventFn(const FWindowResizeEventFn &Func) override;
    void SetCloseEventFn(const FWindowCloseEventFn &Func) override;
    void SetKeyEventFn(const FKeyEventFn &Func) override;
    void SetMouseButtonEventFn(const FMouseButtonEventFn &Func) override;
    void SetMouseMoveEventFn(const FMouseMoveEventFn &Func) override;

    FWindowSizeData GetFramebufferSize() override;

public:
    inline GLFWwindow *GetGLFWWindow() { return NativeHandle; }

private:
    struct FWindowData {
        FWindowCloseEventFn WindowCloseEventFn;
        FWindowResizeEventFn WindowResizeEventFn;
        FKeyEventFn KeyEventFn;
        FMouseButtonEventFn MouseButtonEventFn;
        FMouseMoveEventFn MouseMoveEventFn;
        FSInt32 X;
        FSInt32 Y;
        FSInt32 Width;
        FSInt32 Height;
        FSInt32 FramebufferWidth;
        FSInt32 FramebufferHeight;
        std::string Title;
    };

    FWindowData Data;
    GLFWwindow *NativeHandle = NULL;
};
