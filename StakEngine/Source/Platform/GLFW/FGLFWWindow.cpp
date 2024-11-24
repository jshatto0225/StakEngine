#include "FGLFWWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

bool FGLFWWindow::sGLFWInitialized = false;

FGLFWWindow::FGLFWWindow(const FWindowConfig &Cfg) {
  mData = { NULL, NULL, 0, 0, Cfg.Width, Cfg.Height, Cfg.Title };

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
      Data->Width = Width;
      Data->Height = Height;
      FWindowResizeEvent e(Width, Height);
      Data->WindowResizeEventFn(e);
    }
  );

  glfwSetWindowCloseCallback(mNativeHandle, 
    [](GLFWwindow *Window) {
      auto *Data = static_cast<FWindowData *>(glfwGetWindowUserPointer(Window));
      Data->WindowCloseEventFn();
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

IInput::FMousePosData FGLFWWindow::GetMousePos() {
  FDouble X;
  FDouble Y;
  glfwGetCursorPos(mNativeHandle, &X, &Y);
  return { static_cast<FSInt32>(X), static_cast<FSInt32>(Y) };
}

bool FGLFWWindow::KeyDown(EKeyCode Key) {
  return glfwGetKey(mNativeHandle, static_cast<FUInt16>(Key)) == GLFW_PRESS;
}

bool FGLFWWindow::MouseButtonDown(EMouseCode Button) {
  return glfwGetKey(mNativeHandle, static_cast<FSInt16>(Button)) == GLFW_PRESS;
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
