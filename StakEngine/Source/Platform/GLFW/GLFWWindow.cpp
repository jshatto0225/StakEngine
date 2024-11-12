#include "GLFWWindow.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

namespace Stak {

bool GLFWWindow::sGLFWInitialized = false;

GLFWWindow::GLFWWindow(const WindowConfig &cfg) : mData({ NULL, 0, 0, cfg.width, cfg.height, cfg.title }), mNativeHandle(NULL) {
  if (!sGLFWInitialized) {
    if (!glfwInit()) {
      return;
    }
    sGLFWInitialized = true;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  mNativeHandle = glfwCreateWindow(cfg.width, cfg.height, cfg.title, NULL, NULL);
  glfwSetWindowUserPointer(mNativeHandle, static_cast<void *>(&mData));

  glfwSetWindowSizeCallback(mNativeHandle, [](GLFWwindow *window, i32 width, i32 height) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->width = width;
    data->height = height;
    WindowResizeEvent e(width, height);
    data->eventFunction(e);
    });

  glfwSetWindowPosCallback(mNativeHandle, [](GLFWwindow *window, i32 x, i32 y) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->x = x;
    data->y = y;
    WindowMovedEvent e(x, y);
    data->eventFunction(e);
    });

  glfwSetWindowCloseCallback(mNativeHandle, [](GLFWwindow *window) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    WindowCloseEvent e;
    data->eventFunction(e);
    });
}

GLFWWindow::~GLFWWindow() {
  glfwDestroyWindow(mNativeHandle);
  mNativeHandle = NULL;
}

void GLFWWindow::initImGui() {
  ImGui_ImplGlfw_InitForVulkan(mNativeHandle, true);
}

void GLFWWindow::shutdownImGui() {
  ImGui_ImplGlfw_Shutdown();
}

void GLFWWindow::imGuiNewFrame() {
  ImGui_ImplGlfw_NewFrame();
}

void GLFWWindow::update() {
  glfwPollEvents();
}

WindowSizeData GLFWWindow::getSize() {
  return { mData.width, mData.height };
}

WindowPosData GLFWWindow::getPos() {
  return { mData.x, mData.y };
}

void GLFWWindow::setEventFn(const EventFn &func) {
  mData.eventFunction = func;
}

} // namespace Stak
