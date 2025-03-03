#include "GLFWWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

static bool GLFWInitialized = false;

FGLFWWindow::FGLFWWindow(const FWindowConfig &Cfg) {
    Data = { NULL, NULL, NULL, NULL, NULL, 0, 0, Cfg.Width, Cfg.Height, 0, 0, Cfg.Title };

    if (!GLFWInitialized) {
        if (!glfwInit()) {
            return;
        }
        GLFWInitialized = true;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    NativeHandle = glfwCreateWindow(Cfg.Width, Cfg.Height, Cfg.Title, NULL, NULL);
    glfwSetWindowUserPointer(NativeHandle, static_cast<void *>(&Data));
    glfwGetFramebufferSize(NativeHandle, &Data.FramebufferWidth, &Data.FramebufferHeight);

    glfwSetFramebufferSizeCallback(NativeHandle, [](GLFWwindow* Window, FSInt32 Width, FSInt32 Height) {
        auto* Data = static_cast<FWindowData*>(glfwGetWindowUserPointer(Window));

        Data->FramebufferWidth = Width;
        Data->FramebufferHeight = Height;
    });

    glfwSetFramebufferSizeCallback(NativeHandle, [](GLFWwindow* Window, FSInt32 Width, FSInt32 Height) {
        auto* Data = static_cast<FWindowData*>(glfwGetWindowUserPointer(Window));

        Data->FramebufferWidth = Width;
        Data->FramebufferHeight = Height;
    });

    glfwSetWindowSizeCallback(NativeHandle, [](GLFWwindow *Window, FSInt32 Width, FSInt32 Height) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        Data->Width = Width;
        Data->Height = Height;
        
        if (!Data->WindowResizeEventFn) return;

        FWindowResizeEvent E(Width, Height);
        Data->WindowResizeEventFn(E);
    });

    glfwSetWindowCloseCallback(NativeHandle, [](GLFWwindow *Window) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        if (!Data->WindowCloseEventFn) return;

        Data->WindowCloseEventFn();
    });

    glfwSetKeyCallback(NativeHandle, [](GLFWwindow *Window, FSInt32 Key, FSInt32 Scancode, FSInt32 Action, FSInt32 Mods) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        if (!Data->KeyEventFn) return;

        EInputState State;
        switch (Action) {
        case GLFW_PRESS:
            State = EInputState::DOWN;
            break;
        case GLFW_RELEASE:
            State = EInputState::UP;
            break;
        default:
            return;
        }

        FKeyEvent E(static_cast<EKeyCode>(Key), State);
        Data->KeyEventFn(E);
    });

    glfwSetMouseButtonCallback(NativeHandle, [](GLFWwindow *Window, FSInt32 Button, FSInt32 Action, FSInt32 Mods) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));
      
        if (!Data->MouseButtonEventFn) return;

        EInputState State;
        switch (Action) {
        case GLFW_PRESS:
            State = EInputState::DOWN;
            break;
        case GLFW_RELEASE:
            State = EInputState::UP;
            break;
        default:
            return;
        }

        FMouseButtonEvent E(static_cast<EMouseCode>(Button), State);
        Data->MouseButtonEventFn(E);
    });

    glfwSetCursorPosCallback(NativeHandle, [](GLFWwindow *Window, double X, double Y) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        if (!Data->MouseMoveEventFn) return;

        FMouseMoveEvent E(static_cast<FFloat>(X), static_cast<FFloat>(Y));
        Data->MouseMoveEventFn(E);
    });
}

FGLFWWindow::~FGLFWWindow() {
    glfwDestroyWindow(NativeHandle);
    NativeHandle = NULL;
}

void FGLFWWindow::InitImGui() {
    ImGui_ImplGlfw_InitForVulkan(NativeHandle, true);
}

// TODO: Move to Platform
void FGLFWWindow::ShutdownImGui() {
    ImGui_ImplGlfw_Shutdown();
}

// TODO: Move to Platform
void FGLFWWindow::ImGuiNewFrame() {
    ImGui_ImplGlfw_NewFrame();
}

FWindowSizeData FGLFWWindow::GetSize() {
    return { Data.Width, Data.Height };
}

FWindowPosData FGLFWWindow::GetPos() {
    return { Data.X, Data.Y };
}

void FGLFWWindow::SetResizeEventFn(const FWindowResizeEventFn &Func) {
    Data.WindowResizeEventFn = Func;
}

void FGLFWWindow::SetCloseEventFn(const FWindowCloseEventFn &Func) {
    Data.WindowCloseEventFn = Func;
}

void FGLFWWindow::SetKeyEventFn(const FKeyEventFn &Func) {
    Data.KeyEventFn = Func;
}

void FGLFWWindow::SetMouseButtonEventFn(const FMouseButtonEventFn &Func) {
    Data.MouseButtonEventFn = Func;
}

void FGLFWWindow::SetMouseMoveEventFn(const FMouseMoveEventFn &Func) {
    Data.MouseMoveEventFn = Func;
}

FWindowSizeData FGLFWWindow::GetFramebufferSize() {
    return { Data.FramebufferWidth, Data.FramebufferHeight };
}
