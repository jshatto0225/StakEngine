#pragma once

#include <GLFW/glfw3.h>

#include <string>

#include "Window.h"
#include "Events.h"

struct IRHIViewport;

struct FGLFWWindow final : public IWindow {
    bool Init(const FWindowConfig &Cfg) override;
    void Shutdown() override;

    void InitImGui() override;
    void ImGuiNewFrame() override;
    void ShutdownImGui() override;

    inline GLFWwindow *GetGLFWWindow() { return NativeHandle; }

    GLFWwindow *NativeHandle = nullptr;
};
