#include "FGLFWWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

bool FGLFWWindow::sGLFWInitialized = false;

FGLFWWindow::FGLFWWindow(const FWindowConfig &Cfg) {
  mData = { NULL, NULL, NULL, NULL, NULL, 0, 0, Cfg.Width, Cfg.Height, Cfg.Title };

  if (!sGLFWInitialized) {
    if (!glfwInit()) {
      return;
    }
    sGLFWInitialized = true;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  mNativeHandle = glfwCreateWindow(Cfg.Width, Cfg.Height, Cfg.Title, NULL, NULL);
  glfwSetWindowUserPointer(mNativeHandle, static_cast<void *>(&mData));

  glfwSetWindowSizeCallback(mNativeHandle, 
    [](GLFWwindow *Window, FSInt32 Width, FSInt32 Height) {
      auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

      if (!Data->WindowResizeEventFn) {
        return;
      }

      Data->Width = Width;
      Data->Height = Height;
      FWindowResizeEvent E(Width, Height);
      Data->WindowResizeEventFn(E);
    }
  );

  glfwSetWindowCloseCallback(mNativeHandle, 
    [](GLFWwindow *Window) {
      auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

      if (!Data->WindowCloseEventFn) {
        return;
      }

      Data->WindowCloseEventFn();
    }
  );

  glfwSetKeyCallback(mNativeHandle,
    [](GLFWwindow *Window, FSInt32 Key, FSInt32 Scancode, FSInt32 Action, FSInt32 Mods) {
      auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

      if (!Data->KeyEventFn) {
        return;
      }

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
    }
  );

  glfwSetMouseButtonCallback(mNativeHandle,
    [](GLFWwindow *Window, FSInt32 Button, FSInt32 Action, FSInt32 Mods) {
      auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));
      
      if (!Data->MouseButtonEventFn) {
        return;
      }

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
    }
  );

  glfwSetCursorPosCallback(mNativeHandle,
    [](GLFWwindow *Window, double X, double Y) {
      auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));

      if (!Data->MouseMoveEventFn) {
        return;
      }

      FMouseMoveEvent E(static_cast<FFloat>(X), static_cast<FFloat>(Y));
      Data->MouseMoveEventFn(E);
    }
  );
}

FGLFWWindow::~FGLFWWindow() {
  glfwDestroyWindow(mNativeHandle);
  mNativeHandle = NULL;
}

void FGLFWWindow::InitImGui() {
  ImGui_ImplGlfw_InitForVulkan(mNativeHandle, true);
}

void FGLFWWindow::ShutdownImGui() {
  ImGui_ImplGlfw_Shutdown();
}

void FGLFWWindow::ImGuiNewFrame() {
  ImGui_ImplGlfw_NewFrame();
}

void FGLFWWindow::Update() {
  glfwPollEvents();
}

FWindowSizeData FGLFWWindow::GetSize() {
  return { mData.Width, mData.Height };
}

FWindowPosData FGLFWWindow::GetPos() {
  return { mData.X, mData.Y };
}

void FGLFWWindow::SetResizeEventFn(const FWindowResizeEventFn &Func) {
  mData.WindowResizeEventFn = Func;
}

void FGLFWWindow::SetCloseEventFn(const FWindowCloseEventFn &Func) {
  mData.WindowCloseEventFn = Func;
}

void FGLFWWindow::SetKeyEventFn(const FKeyEventFn &Func) {
  mData.KeyEventFn = Func;
}

void FGLFWWindow::SetMouseButtonEventFn(const FMouseButtonEventFn &Func) {
  mData.MouseButtonEventFn = Func;
}

void FGLFWWindow::SetMouseMoveEventFn(const FMouseMoveEventFn &Func) {
  mData.MouseMoveEventFn = Func;
}
