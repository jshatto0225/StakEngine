#include "GLFWWindow.h"

namespace Stak {

bool GLFWWindow::s_GLFWInitialized = false;

GLFWWindow::GLFWWindow(const WindowConfig &cfg) : m_NativeHandle(NULL) {
  if (!s_GLFWInitialized) {
    if (!glfwInit()) {
      return;
    }
    s_GLFWInitialized = true;
  }

  m_NativeHandle = glfwCreateWindow(cfg.Width, cfg.Height, cfg.Title, NULL, NULL);
  glfwSetWindowUserPointer(m_NativeHandle, static_cast<void *>(&m_Data));

  glfwSetWindowSizeCallback(m_NativeHandle, [](GLFWwindow *window, i32 width, i32 height) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->Width = width;
    data->Height = height;
    WindowResizeEvent e(width, height);
    data->EventFunction(e);
    });

  glfwSetWindowPosCallback(m_NativeHandle, [](GLFWwindow *window, i32 x, i32 y) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    data->X = x;
    data->Y = y;
    WindowMovedEvent e(x, y);
    data->EventFunction(e);
    });

  glfwSetWindowCloseCallback(m_NativeHandle, [](GLFWwindow *window) {
    WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(window));
    WindowCloseEvent e;
    data->EventFunction(e);
    });
}

GLFWWindow::~GLFWWindow() {
  glfwDestroyWindow(m_NativeHandle);
  m_NativeHandle = NULL;
}

void GLFWWindow::Update() {
  glfwPollEvents();
}

WindowSizeData GLFWWindow::GetWindowSize() {
  return { m_Data.Width, m_Data.Height };
}

WindowPosData GLFWWindow::GetWindowPos() {
  return { m_Data.X, m_Data.Y };
}

void GLFWWindow::SetEventFn(const EventFn &func) {
  m_Data.EventFunction = func;
}

} // namespace Stak