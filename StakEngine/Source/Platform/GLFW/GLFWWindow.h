#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "Window.h"
#include "Events.h"

class IRHIViewport;

class FGLFWWindow final : public IWindow {
public:
    bool Init(const FWindowConfig &Cfg) override;
    void Shutdown() override;

    FWindowSizeData GetSize() override;
    FWindowPosData GetPos() override;

    void InitImGui() override;
    void ImGuiNewFrame() override;
    void ShutdownImGui() override;

    void SetEventFn(const FEventFn &Func) override;

    FWindowSizeData GetFramebufferSize() override;

    TRef<IRHIViewport> GetRHIViewport() override;

public:
    inline GLFWwindow *GetGLFWWindow() { return NativeHandle; }

private:
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
    GLFWwindow *NativeHandle = nullptr;
};
