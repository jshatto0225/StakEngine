#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include <glfw/glfw3.h>

#include "RHI.h"
#include "Window.h"
#include "Log.h"

static bool GlfwInitalized = false;

bool PlatformInit() {
    if (!glfwInit()) {
        return false;
    }

    GlfwInitalized = true;

    return true;
}

void PlatformShutdown() {
    assert(GlfwInitalized);

    glfwTerminate();
    GlfwInitalized = false;
}

bool PlatformOpenWindow(FWindow *Window, const FWindowConfig *Cfg) {
    assert(GlfwInitalized);
    assert(Window);

    if (Window->Open) {
        SK_LOG_WARN("Window already open");
        return true;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    Window->Width   = Cfg->Width;
    Window->Height  = Cfg->Height;
    Window->Title   = Cfg->Title;
    Window->EventFn = Cfg->EventFn;

    auto Handle = glfwCreateWindow(Cfg->Width, Cfg->Height, Cfg->Title, nullptr, nullptr);

    if (!Handle) {
        SK_LOG_ERROR("Failed to create glfw window");
        return false;
    }

    Window->PlatformHandle = reinterpret_cast<FHandle>(Handle);

    glfwSetWindowUserPointer(Handle, static_cast<void *>(Window));
    glfwGetFramebufferSize(Handle, &Window->FramebufferWidth, &Window->FramebufferHeight);

    glfwSetFramebufferSizeCallback(Handle, [](GLFWwindow *Window, FSInt32 Width, FSInt32 Height) {
        auto *Data = static_cast<FWindow *>(glfwGetWindowUserPointer(Window));

        Data->FramebufferWidth = Width;
        Data->FramebufferHeight = Height;

        GRHI.NotifySwapchainOfResize(Data->Swapchain);
    });

    glfwSetWindowSizeCallback(Handle, [](GLFWwindow *Window, FSInt32 Width, FSInt32 Height) {
        auto *Data = static_cast<FWindow *>(glfwGetWindowUserPointer(Window));

        Data->Width = Width;
        Data->Height = Height;

        if (!Data->EventFn) return;

        FEvent E = {};
        E.Type = EEventType::WINDOW_RESIZE;
        E.WRE.Width = Width;
        E.WRE.Height = Height;
        Data->EventFn(&E);
    });

    glfwSetWindowCloseCallback(Handle, [](GLFWwindow *Window) {
        auto *Data = static_cast<FWindow *>(glfwGetWindowUserPointer(Window));

        if (!Data->EventFn) return;

        FEvent E = {};
        E.Type = EEventType::WINDOW_CLOSE;
        Data->EventFn(&E);
    });

    glfwSetKeyCallback(Handle, [](GLFWwindow *Window, FSInt32 Key, FSInt32 Scancode, FSInt32 Action, FSInt32 Mods) {
        auto *Data = static_cast<FWindow *>(glfwGetWindowUserPointer(Window));

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

    glfwSetMouseButtonCallback(Handle, [](GLFWwindow *Window, FSInt32 Button, FSInt32 Action, FSInt32 Mods) {
        auto *Data = static_cast<FWindow *>(glfwGetWindowUserPointer(Window));

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

    glfwSetCursorPosCallback(Handle, [](GLFWwindow *Window, double X, double Y) {
        auto *Data = static_cast<FWindow *>(glfwGetWindowUserPointer(Window));

        if (!Data->EventFn) return;

        FEvent E = {};
        E.Type = EEventType::MOUSE_MOVE;
        E.MME.X = static_cast<FFloat>(X);
        E.MME.Y = static_cast<FFloat>(Y);
        Data->EventFn(&E);
    });

    Window->Swapchain = GRHI.CreateSwapchain(Window);
    if (!Window->Swapchain) {
        SK_LOG_ERROR("Failed to create viewport for window");
        return false;
    }

    Window->Open = true;

    return true;
}

void PlatformCloseWindow(FWindow *Window) {
    assert(Window);

    if (!Window->Open) {
        SK_LOG_WARN("Window already closed");
        return;
    }

    auto Glfw = (GLFWwindow *)Window->PlatformHandle;
    assert(Glfw);

    GRHI.DestroySwapchain(&Window->Swapchain);

    glfwDestroyWindow(Glfw);

    Window->PlatformHandle = 0;

    Window->Open = false;
}

bool PlatformInitImGui(const FWindow *Window) {
    assert(Window);

    if (!Window->Open) {
        SK_LOG_ERROR("Failed to initialize imgui, window was not open");
        return false;
    }

    auto Glfw = reinterpret_cast<GLFWwindow *>(Window->PlatformHandle);
    assert(Glfw);

    if (!ImGui_ImplGlfw_InitForVulkan(Glfw, true)) {
        SK_LOG_ERROR("Failed to initialize imgui");
        return false;
    }

    return true;
}

void PlatformImGuiNewFrame() {
    ImGui_ImplGlfw_NewFrame();
}

void PlatformShutdownImGui() {
    ImGui_ImplGlfw_Shutdown();
}

void PlatformProcessMessages() {
    glfwPollEvents();
}

void PlatformEnableRawInput(const FWindow *Window) {
    assert(Window);

    if (!Window->Open) {
        SK_LOG_WARN("Cannot enable raw input, window is not open");
        return;
    }

    auto Glfw = reinterpret_cast<GLFWwindow *>(Window->PlatformHandle);
    assert(Glfw);

    glfwSetInputMode(Glfw, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void PlatformDisableRawInput(const FWindow *Window) {
    assert(Window);

    if (!Window->Open) {
        SK_LOG_WARN("Cannot disable raw input, window is not open");
        return;
    }

    auto Glfw = reinterpret_cast<GLFWwindow *>(Window->PlatformHandle);
    assert(Glfw);



    glfwSetInputMode(Glfw, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

void PlatformSetCursorVisibility(const FWindow *Window, ECursorVisibility Visibility) {
    assert(Window);

    if (!Window->Open) {
        SK_LOG_WARN("Cannot disable raw input, window is not open");
        return;
    }

    auto Glfw = reinterpret_cast<GLFWwindow *>(Window->PlatformHandle);
    assert(Glfw);

    FSInt32 GlfwVisibility;
    switch (Visibility) {
    case ECursorVisibility::NORMAL:
        GlfwVisibility = GLFW_CURSOR_NORMAL;
        break;
    case ECursorVisibility::HIDDEN:
        GlfwVisibility = GLFW_CURSOR_HIDDEN;
        break;
    case ECursorVisibility::DISABLED:
        GlfwVisibility = GLFW_CURSOR_DISABLED;
        break;
    default:
        return;
    }

    glfwSetInputMode(Glfw, GLFW_CURSOR, GlfwVisibility);
}
