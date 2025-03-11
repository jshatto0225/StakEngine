#include "GLFWWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include "RHIViewport.h"
#include "RHI.h"

static bool GLFWInitialized = false;

bool FGLFWWindow::Init(const FWindowConfig &Cfg) {
    Data = { nullptr, 0, 0, Cfg.Width, Cfg.Height, 0, 0, Cfg.Title };

    if (!GLFWInitialized) {
        if (!glfwInit()) {
            return false;
        }
        GLFWInitialized = true;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    NativeHandle = glfwCreateWindow(Cfg.Width, Cfg.Height, Cfg.Title, nullptr, nullptr);
    glfwSetWindowUserPointer(NativeHandle, static_cast<void *>(&Data));
    glfwGetFramebufferSize(NativeHandle, &Data.FramebufferWidth, &Data.FramebufferHeight);

    glfwSetFramebufferSizeCallback(NativeHandle, [](GLFWwindow* Window, FSInt32 Width, FSInt32 Height) {
        auto* Data = static_cast<FWindowData*>(glfwGetWindowUserPointer(Window));

        Data->Viewport->OnFramebufferResize();

        Data->FramebufferWidth = Width;
        Data->FramebufferHeight = Height;
    });

    glfwSetWindowSizeCallback(NativeHandle, [](GLFWwindow *Window, FSInt32 Width, FSInt32 Height) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        Data->Width = Width;
        Data->Height = Height;
        
        if (!Data->EventFn) return;

        FEvent E = {};
        E.Type = EEventType::WINDOW_RESIZE;
        E.WRE.Width = Width;
        E.WRE.Height = Height;
        Data->EventFn(&E);
    });

    glfwSetWindowCloseCallback(NativeHandle, [](GLFWwindow *Window) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        if (!Data->EventFn) return;

        FEvent E = {};
        E.Type = EEventType::WINDOW_CLOSE;
        Data->EventFn(&E);
    });

    glfwSetKeyCallback(NativeHandle, [](GLFWwindow *Window, FSInt32 Key, FSInt32 Scancode, FSInt32 Action, FSInt32 Mods) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        if (!Data->EventFn) return;

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

        FEvent E = {};
        E.Type = EEventType::KEY;
        E.KE.Key = static_cast<EKeyCode>(Key);
        E.KE.State = State;
        Data->EventFn(&E);
    });

    glfwSetMouseButtonCallback(NativeHandle, [](GLFWwindow *Window, FSInt32 Button, FSInt32 Action, FSInt32 Mods) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));
      
        if (!Data->EventFn) return;

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

        FEvent E = {};
        E.Type = EEventType::MOUSE_BUTTON;
        E.MBE.Button = static_cast<EMouseCode>(Button);
        E.MBE.State = State;
        Data->EventFn(&E);
    });

    glfwSetCursorPosCallback(NativeHandle, [](GLFWwindow *Window, double X, double Y) {
        auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

        if (!Data->EventFn) return;

        FEvent E = {};
        E.Type = EEventType::MOUSE_MOVE;
        E.MME.X = static_cast<FFloat>(X);
        E.MME.Y = static_cast<FFloat>(Y);
        Data->EventFn(&E);
    });

    Data.Viewport = RHICreateViewport(NativeHandle);
    if (!Data.Viewport->Init()) {
        SK_LOG_ERROR("Failed to create viewport for window");
        return false;
    }

    return true;
}

void FGLFWWindow::Shutdown() {
    Data.Viewport->Shutdown();
    glfwDestroyWindow(NativeHandle);
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

void FGLFWWindow::SetEventFn(FEventFn Func) {
    Data.EventFn = Func;
}

FWindowSizeData FGLFWWindow::GetFramebufferSize() {
    return { Data.FramebufferWidth, Data.FramebufferHeight };
}

TRef<IRHIViewport> FGLFWWindow::GetRHIViewport() {
    return Data.Viewport;
}